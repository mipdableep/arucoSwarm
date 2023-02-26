//
// created by ido Talmor
//

#ifndef Aruco_Lib_cpp
#define Aruco_Lib_cpp

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

class aruco_utils
{
public:
      aruco_utils(std::string yamlCalibrationPath, float currentMarkerSize, int id_to_follow);

      void printVector(std::vector<cv::Vec3d> vec);

      cv::Mat calculate_6_DOF(cv::Mat img);

      int ID = -1;
      int correct_index = 0;
      bool Target_found = false;

      bool imshow = false;
      int id_to_follow;
      bool videoCap;

      float Tx;
      float Ty;
      float Tz;

      float yaw;
      float pitch;

      cv::Mat frame;

private:
      
      bool stop;
      float currentMarkerSize;
      std::string yamlCalibrationPath;
      
      // vars for calculate_6_DOF()

      std::vector<cv::Mat> cameraParams;

      const cv::Ptr<cv::aruco::Dictionary> dictionary =
      cv::aruco::getPredefinedDictionary (cv::aruco::DICT_ARUCO_ORIGINAL /*DICT_4X4_100*/);

      cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();

      //ids of arucos in frame
      std::vector<int> ids;
      // corner list of arucos in frame
      std::vector<std::vector<cv::Point2f>> corners;

      std::vector<cv::Mat> getCameraCalibration(const std::string &path);

      double RADIANS_TO_DEGREESE = (180/3.141592653589793238463);
      double PI = 3.141592653589793238463;

      // void getCameraFeed();
};

#endif