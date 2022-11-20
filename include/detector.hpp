#ifndef DETECTOR_H_
#define DETECTOR_H_

#include <opencv2/core/hal/interface.h>
#include <tensorflow/lite/interpreter.h>

#include <boost/lockfree/spsc_queue.hpp>
#include <cstdint>
#include <memory>
#include <opencv2/core/mat.hpp>
#include <string>
#include <thread>
#include <vector>

class Detector {
    std::string model_path;
    boost::lockfree::spsc_queue<std::vector<uchar>>& frame_queue;
    std::thread detection_thread;
    bool end_detection = false;
    std::unique_ptr<tflite::Interpreter> interpreter;
    boost::lockfree::spsc_queue<std::vector<int>> classes_queue;

    void detection_handler(int model_threads_num);
    void push_new_labels_to_queue();
    bool prepare_model(int model_threads_num);

    cv::Mat get_current_frame_from_queue();

    void fill_array_with_mat(uint8_t* in, const cv::Mat& src);

   public:
    Detector(
        const std::string& model_path,
        boost::lockfree::spsc_queue<std::vector<unsigned char>>& frame_queue);
    ~Detector();

    void start_detection(int model_threads_num = 4);
    void finish_detection();

    std::vector<int> get_classes_in_frame();
    boost::lockfree::spsc_queue<std::vector<int>>& get_classes_queue();
};

#endif  // DETECTOR_H_
