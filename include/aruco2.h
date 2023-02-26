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
      aruco_utils(std::string &yamlCalibrationPath, int cameraPort,
            float currentMarkerSize);

      aruco_utils(std::string &yamlCalibrationPath, int cameraPort,
            float currentMarkerSize, int cam_fps);

      ~aruco_utils();

      aruco_utils(std::string &yamlCalibrationPath, std::string &cameraString,
            float currentMarkerSize);

      void trackMarkerThread();

      void printVector(std::vector<cv::Vec3d> vec);

      void calculate_6_DOF(cv::Mat img);

      int ID = -1;
      int correct_index = 0;
      bool Target_found = false;

      bool imshow = false;
      int id_to_follow;
      bool videoCap;
      
      cv::Mat objPoints;

private:
      
      bool stop;
      float currentMarkerSize;
      std::string yamlCalibrationPath;
      
      // vars for calculate_6_DOF()
      const std::vector<cv::Mat> cameraParams = getCameraCalibration(yamlCalibrationPath);
      const cv::Ptr<cv::aruco::Dictionary> dictionary =
      cv::aruco::getPredefinedDictionary (cv::aruco::DICT_ARUCO_ORIGINAL /*DICT_4X4_100*/);

      cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();

      //ids of arucos in frame
      std::vector<int> ids;
      // corner list of arucos in frame
      std::vector<std::vector<cv::Point2f>> corners;


      std::shared_ptr<cv::Mat> frame;
      std::thread cameraThread;
      std::thread arucoThread;
      std::shared_ptr<bool> holdCamera;

      std::shared_ptr<cv::VideoCapture> capture;

      boost::lockfree::spsc_queue<std::vector<uchar>> frame_queue;

      long amountOfUSleepForTrackMarker = 5000;

      std::vector<cv::Mat> getCameraCalibration(const std::string &path);

      double RADIANS_TO_DEGREESE = (180/3.141592653589793238463);


      void getCameraFeed();
};

#endif