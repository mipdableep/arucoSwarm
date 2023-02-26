#include "../include/aruco2.h"
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

// TODO: implement "frame && !frame->empty()" to check the mat passed
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

    std::cout<<"got to end" << std::endl;

    float Rx = R_vec[0];
    float Ry = R_vec[1];
    float Rz = R_vec[2];

    Tx = -T_vec[0]*100;
    Ty = -T_vec[1]*100;
    Tz = T_vec[2]*100;
    yaw = -atan2(Rx, Rz) * RADIANS_TO_DEGREESE;
    pitch = 90 - atan2(Rz, Ry) * RADIANS_TO_DEGREESE;

    return img;
}

void aruco_utils::printVector(std::vector<cv::Vec3d> vec) {
    // print all values of localTvecs
    std::for_each(begin(vec), end(vec), [](cv::Vec3d &element) { std::cout << element << ","; });
    if (!vec.empty()) {
        std::cout << std::endl;
    }
}

aruco_utils::aruco_utils(std::string yamlCalibrationPath, float currentMarkerSize, int id_to_follow) {
    this->yamlCalibrationPath = yamlCalibrationPath;
    this->currentMarkerSize = currentMarkerSize;
    this->id_to_follow = id_to_follow;

    cameraParams = getCameraCalibration(yamlCalibrationPath);
}
