#include "../include/arucoUtils.h"
#include <fstream>

std::vector<cv::Mat> aruco_utils::getCameraCalibration(const std::string &path) {
    cv::FileStorage fs(path, cv::FileStorage::READ);
    if (!fs.isOpened())
        throw std::runtime_error(
            "CameraParameters::readFromXMLFile could not open file:" + path);
    int w = -1, h = -1;
    cv::Mat MCamera, MDist;

    fs["image_width"] >> w;
    fs["image_height"] >> h;
    fs["distortion_coefficients"] >> MDist;
    fs["camera_matrix"] >> MCamera;

    if (MCamera.cols == 0 || MCamera.rows == 0) {
        fs["Camera_Matrix"] >> MCamera;
        if (MCamera.cols == 0 || MCamera.rows == 0)
            throw cv::Exception(
                9007,
                "File :" + path + " does not contains valid camera matrix",
                "CameraParameters::readFromXML", __FILE__, __LINE__);
    }

    if (w == -1 || h == 0) {
        fs["image_Width"] >> w;
        fs["image_Height"] >> h;
        if (w == -1 || h == 0)
            throw cv::Exception(
                9007,
                "File :" + path + " does not contains valid camera dimensions",
                "CameraParameters::readFromXML", __FILE__, __LINE__);
    }
    if (MCamera.type() != CV_32FC1) MCamera.convertTo(MCamera, CV_32FC1);

    if (MDist.total() < 4) {
        fs["Distortion_Coefficients"] >> MDist;
        if (MDist.total() < 4)
            throw cv::Exception(
                9007,
                "File :" + path +
                    " does not contains valid distortion_coefficients",
                "CameraParameters::readFromXML", __FILE__, __LINE__);
    }
    MDist.convertTo(MDist, CV_32FC1);
    std::vector<cv::Mat> newCameraParams = {MCamera, MDist};
    return newCameraParams;
}

cv::Mat aruco_utils::calculate_6_DOF(cv::Mat img)
{
    cv::aruco::detectMarkers(img, dictionary, corners, ids);

    // if no arucos found
    if (ids.empty()){
        correct_index = -9;
        Target_found = false;
        return img;
    }
    
    cv::aruco::drawDetectedMarkers(img, corners, ids);

    auto it = std::find(ids.begin(), ids.end(), id_to_follow);
    
    //if right aruco not found
    if (it == ids.end()) {
        correct_index = -9;
        Target_found = false;
        return img;
    }
    correct_index = it - ids.begin();

    std::vector<cv::Vec3d> localRvecs, localTvecs;
    cv::aruco::estimatePoseSingleMarkers(corners, currentMarkerSize, cameraParams[0], cameraParams[1], localRvecs, localTvecs);
    
    if (localRvecs.empty()){
        Target_found = false;
        return img;
    }

    // draw Target aruco axis
    cv::drawFrameAxes(img, cameraParams[0], cameraParams[1], localRvecs[correct_index], localTvecs[correct_index], currentMarkerSize);


    cv::Mat rotation_mat = cv::Mat::eye(3, 3, CV_64FC1);
    try {
        cv::Rodrigues(localRvecs[correct_index], rotation_mat);
    } catch (...) {
        std::cout << "could not convert vector to mat"<< std::endl;
        return img;
    }

    cv::Mat forward_mat = (cv::Mat_<double>(3, 1) << 0, 0, -1);

    cv::Mat R_vec_mult = rotation_mat * forward_mat;
    cv::Vec3d T_vec = localTvecs[correct_index];

    cv::Vec3d R_vec (R_vec_mult.at<double>(0), R_vec_mult.at<double>(1), R_vec_mult.at<double>(2));

    float Rx = R_vec[0];
    float Ry = R_vec[1];
    float Rz = R_vec[2];

    Tx = -T_vec[0]*100;
    Tz = -T_vec[1]*100;
    Ty = T_vec[2]*100;
    yaw = -atan2(Rx, Rz) * RADIANS_TO_DEGREESE;
    pitch = 90 - atan2(Rz, Ry) * RADIANS_TO_DEGREESE;

    Target_found = true;

    return img;
}

void aruco_utils::main_aruco_loop(){
    std::cout<<"started main aruco loop"<<std::endl;

    int frame_counter = 0;
    std::vector<uchar> current_frame;

    while (!capture.isOpened())
        usleep(20000);

    cv::Mat img;

    int w = capture.get(3);
    int h = capture.get(4);


    while (run_main_loop)
    {
        // get current frame data
        while (!frame_queue.pop(current_frame)) {
            usleep(200);
        }
        cv::Mat frame(h, w, CV_8UC3, current_frame.data());

        if (frame.empty())
            continue;

        img = calculate_6_DOF(frame);

        // TODO: imshow
        if (imshow){
            cv::imshow("frame", img);
            exit = (cv::waitKey(1) == (int)'q' ? true : false);
        }

        // TODO: save video
        if (save_video && frame_counter%7 == 0)
            cv::imwrite(save_run_path + std::to_string(frame_counter) + ".jpg", img);

        frame_counter ++;
        usleep(usleep_between_frames);
    }
}

/// @brief opens video capture with string
/// @param udp_string cap.open(udp_string)
/// @param capture_width new width, set to 0 (or less) for auto
/// @param capture_height new height, set to 0 (or less) for auto
void aruco_utils::open_video_cap(std::string udp_string, int capture_width, int capture_height)
{
    std::cout << "started open_video_cap" << std::endl;
    capture.open(udp_string);

    if (capture_width > 0 && capture_height > 0){
        capture.set(3, capture_width);
        capture.set(4, capture_height);
    }

    camera_feed_to_queue();
}

/// @brief opens video capture with port
/// @param udp_string cap.open(udp_string)
/// @param capture_width new width, set to 0 (or less) for auto
/// @param capture_height new height, set to 0 (or less) for auto
void aruco_utils::open_video_cap(int camera_port, int capture_width, int capture_height)
{
    std::cout << "started open_video_cap" << std::endl;
    capture.open(camera_port);

    if (capture_width > 0 && capture_height > 0){
        capture.set(3, capture_width);
        capture.set(4, capture_height);
    }

    camera_feed_to_queue();
}


void aruco_utils::camera_feed_to_queue() {

    std::cout << "started camera to queue" << std::endl;

    while (!capture.isOpened())
        usleep(40000);

    while (run_camera) {
        if (pause_capture_to_queue) {
            usleep(100000);
            continue;
        }
        cv::Mat temp_frame;
        capture.read(temp_frame);

        std::vector<uchar> frame_vec;
        // converting frame to vector, and pushing frame to queue
        frame_vec.assign(
            temp_frame.data,
            temp_frame.data + temp_frame.total() * temp_frame.channels());

        frame_queue.push(frame_vec);
    }
}

// * this part is important - " : frame_queue(3)" - initialization of boost::lockfree::spsc_queue
aruco_utils::aruco_utils(std::string yamlCalibrationPath, float currentMarkerSize, int id_to_follow) : frame_queue(3) {
    this->yamlCalibrationPath = yamlCalibrationPath;
    this->currentMarkerSize = currentMarkerSize;
    this->id_to_follow = id_to_follow;

    cameraParams = getCameraCalibration(yamlCalibrationPath);
    // TODO: add save_run path
}

aruco_utils::~aruco_utils(){
    run_camera = false;
    run_main_loop = false;

}