#include "detector.hpp"

#include <opencv2/core/hal/interface.h>
#include <tensorflow/lite/core/c/c_api_types.h>
#include <tensorflow/lite/core/c/common.h>
#include <tensorflow/lite/core/interpreter_builder.h>
#include <tensorflow/lite/core/kernels/register.h>
#include <tensorflow/lite/core/model_builder.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

Detector::Detector(
    const std::string& model_path,
    boost::lockfree::spsc_queue<std::vector<unsigned char>>& frame_queue)
    : model_path(model_path), frame_queue(frame_queue), classes_queue(1) {}

Detector::~Detector() { finish_detection(); }

boost::lockfree::spsc_queue<std::vector<int>>& Detector::get_classes_queue() {
    return classes_queue;
}

bool Detector::prepare_model(int model_threads_num) {
    std::unique_ptr<tflite::FlatBufferModel> model =
        tflite::FlatBufferModel::BuildFromFile(model_path.c_str());
    if (model == nullptr) return false;

    // This assumes we enable XNNPACK on compilation
    tflite::ops::builtin::BuiltinOpResolver resolver;
    tflite::InterpreterBuilder builder(*model, resolver);
    builder(&interpreter);
    if (interpreter == nullptr) return false;

    if (interpreter->AllocateTensors() != kTfLiteOk) return false;

    return interpreter->SetNumThreads(model_threads_num) == kTfLiteOk;
}

cv::Mat Detector::get_current_frame_from_queue() {
    std::vector<uchar> current_frame;

    while (frame_queue.empty()) std::this_thread::sleep_for(20ms);

    frame_queue.pop(current_frame);
    return cv::Mat(480, 640, CV_8UC3, current_frame.data());
}

void Detector::fill_array_with_mat(uint8_t* in, const cv::Mat& src) {
    int n = 0, nc = src.channels(), ne = src.elemSize();
    for (int y = 0; y < src.rows; ++y)
        for (int x = 0; x < src.cols; ++x)
            for (int c = 0; c < nc; ++c)
                in[n++] = src.data[y * src.step + x * ne + c];
}

void Detector::push_new_labels_to_queue() {
    const float score_threshold = 0.5;
    const std::vector<int>& outputs = interpreter->outputs();

    // We ignore bboxes for the time being
    const TfLiteTensor* classes = interpreter->tensor(outputs[1]);
    const float* classes_data = classes->data.f;
    const TfLiteTensor* scores = interpreter->tensor(outputs[2]);
    const float* scores_data = scores->data.f;

    const int classes_size = classes->dims->data[classes->dims->size - 1];
    const int scores_size = scores->dims->data[scores->dims->size - 1];

    if (classes_size != scores_size) return;

    std::vector<int> classes_detected;

    for (int i = 0; i < classes_size; ++i) {
        if (scores_data[i] > score_threshold)
            classes_detected.push_back(classes_data[i] + 1);
    }

    classes_queue.push(classes_detected);
}

void Detector::detection_handler(int model_threads_num) {
    if (!prepare_model(model_threads_num)) return;
    const int input = interpreter->inputs()[0];
    const TfLiteIntArray* dims = interpreter->tensor(input)->dims;
    const int in_height = dims->data[1];
    const int in_width = dims->data[2];
    const int in_channels = dims->data[3];
    // We will currently support only uint8 models
    uint8_t* input_typed = interpreter->typed_tensor<uint8_t>(input);

    while (!end_detection) {
        cv::Mat current_frame = get_current_frame_from_queue();
        cv::resize(current_frame, current_frame, cv::Size(in_height, in_width),
                   cv::INTER_CUBIC);
        fill_array_with_mat(input_typed, current_frame);
        if (interpreter->Invoke() == kTfLiteOk) push_new_labels_to_queue();
    }
}

void Detector::start_detection(int model_threads_num) {
    detection_thread =
        std::thread(&Detector::detection_handler, this, model_threads_num);
}

void Detector::finish_detection() {
    end_detection = true;
    detection_thread.join();
}
