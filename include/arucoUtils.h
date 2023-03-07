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
      
      void close();

      cv::Mat calculate_6_DOF(cv::Mat img);

      void main_aruco_loop();

      void camera_feed_to_queue();

      // for drone
      void open_video_cap(std::string udp_string, int capture_width = 0, int capture_height = 0);
      // for wiered camera
      void open_video_cap(int camera_port, int capture_width = 0, int capture_height = 0);

      int ID = -1;
      int correct_index = 0;
      bool Target_found = false;

      int id_to_follow;

      bool imshow = false;
      bool save_video;
      std::string save_run_path;

      float Tx;
      float Ty;
      float Tz;

      float yaw;
      float pitch;

      bool exit = false;


private:
      
      std::vector<cv::Mat> getCameraCalibration(const std::string &path);
      
      int usleep_between_frames = 50000;

      bool run_main_loop = true;
      bool run_camera = true;

      bool pause_capture_to_queue = false;

      float currentMarkerSize;
      std::string yamlCalibrationPath;

      // frame queue (1 pusher 1 consumer)
      boost::lockfree::spsc_queue<std::vector<uchar>> frame_queue;

      cv::VideoCapture capture;

      // vars for calculate_6_DOF()

      std::vector<cv::Mat> cameraParams;

      const cv::Ptr<cv::aruco::Dictionary> dictionary =
      cv::aruco::getPredefinedDictionary (cv::aruco::DICT_ARUCO_ORIGINAL /*DICT_4X4_100*/);

      cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();

      //ids of arucos in frame
      std::vector<int> ids;
      // corner list of arucos in frame
      std::vector<std::vector<cv::Point2f>> corners;


      double RADIANS_TO_DEGREESE = (180/3.141592653589793238463);
      double PI = 3.141592653589793238463;

      // void getCameraFeed();
};

#endif