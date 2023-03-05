
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
#include "src/arucoUtils.cpp"

void webcamTest(){
        aruco_utils util("/home/fares/rbd/projects/aruco_swarm/arucoSwarm/calib/webcam.yaml", 0.07, 145);

    int counter = 0;

    cv::Mat input_img;
    int key;
    cv::VideoCapture cap(0);

    while (key != 'q'){

        cap.read(input_img);

        if (input_img.empty()){
            sleep(1);
            continue;
        }

        cv::Mat frame = util.calculate_6_DOF(input_img);
        if (util.correct_index == -9){
            cv::imshow("Camera", frame);
            cv::waitKey(1);
            usleep(20000);
            continue;
        }

        // std::cout<<"Tx: " << util.Tx;
        // std::cout<<"  Ty: " << util.Ty;
        // std::cout<<"  Tz: " << util.Tz;
        std::cout<<"  yaw: " << util.yaw;
        std::cout<<"  pitch: " << util.pitch;
        std::cout << std::endl;
        
        cv::imshow("Camera", frame);
        key = cv::waitKey(1);
    }
}

int main()
{
    // get_vid();
    // exit(0);

    return 0;
}