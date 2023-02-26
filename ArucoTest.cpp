
#include <boost/lockfree/spsc_queue.hpp>
#include <iostream>
#include <memory>
#include <opencv2/aruco.hpp>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>
#include <algorithm>
#include <opencv2/core/hal/interface.h>
#include <unistd.h>
#include <opencv2/highgui.hpp>
#include <fstream>


float currentMarkerSize;
std::string yamlCalibrationPath;
bool Target_found
// vars for calculate_6_DOF()
const std::vector<cv::Mat> cameraParams = ;
const cv::Ptr<cv::aruco::Dictionary> dictionary =
cv::aruco::getPredefinedDictionary (cv::aruco::DICT_ARUCO_ORIGINAL /*DICT_4X4_100*/);

cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();

//ids of arucos in frame
std::vector<int> ids;
// corner list of arucos in frame
std::vector<std::vector<cv::Point2f>> corners;

int correct_index = 0;
int id_to_follow = 100;


// TODO: implement "frame && !frame->empty()" to check the mat passed
void calculate_6_DOF(cv::Mat img)
{
    cv::aruco::detectMarkers(img, dictionary, corners, ids);

    // if no arucos found
    if (ids.empty()){
        correct_index = -9;
        Target_found = false;
        return;
    }
    
    cv::aruco::drawDetectedMarkers(img, corners, ids);

    auto it = std::find(ids.begin(), ids.end(), id_to_follow);
    
    //if right aruco not found
    if (it == ids.end()) {
        correct_index = -9;
        Target_found = false;
        return;
    }
    correct_index = it - ids.begin();

    std::vector<cv::Vec3d> localRvecs, localTvecs;
    cv::aruco::estimatePoseSingleMarkers(corners, currentMarkerSize, cameraParams[0], cameraParams[1], localRvecs, localTvecs);
    
    if (localRvecs.empty()){
        Target_found = false;
        return;
    }

    // draw Target aruco axis
    cv::drawFrameAxes(img, cameraParams[0], cameraParams[1], localRvecs[correct_index], localTvecs[correct_index], currentMarkerSize);

    cv::Mat rotation_mat = cv::Mat::eye(3, 3, CV_64FC1);
    try {
        cv::Rodrigues(localRvecs[correct_index], rotation_mat);
    } catch (...) {
        std::cout << "could not convert vector to mat"<< std::endl;
        return;
    }

    cv::Mat forward_mat = (cv::Mat_<double>(3, 1) << 0, 0, -1);

    cv::Mat R_vec_mult = rotation_mat * forward_mat;
    cv::Vec3d T_vec = localTvecs[correct_index];

    cv::Vec3d R_vec (R_vec_mult.at<double>(0), R_vec_mult.at<double>(1), R_vec_mult.at<double>(2));

    float Rx = R_vec[0];
    float Ry = R_vec[1];
    float Rz = R_vec[2];

    float Tx = T_vec[0]*100;
    float Ty = T_vec[1]*100;
    float Tz = T_vec[2]*100;

    float yaw = atan2(Rx, Ry) * RADIANS_TO_DEGREESE;
    float pitch = atan2(Rz, sqrt(Rx*Rx + Ry*Ry)) * RADIANS_TO_DEGREESE;

}

