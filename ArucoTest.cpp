
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
#include "src/aruco2.cpp"

void get_vid() {
    // Open the video capture device at port 0
    cv::VideoCapture cap(0);
    if(!cap.isOpened()) {
        std::cerr << "Failed to open video capture device" << std::endl;
        return;
    }

    // Create a window to display the frames
    cv::namedWindow("Camera", cv::WINDOW_NORMAL);

    int counter = 0;
    // Loop over the frames
    while(true) {
        // Capture a frame
        cv::Mat frame;
        cap >> frame;

        // Check if the frame was captured successfully
        if(frame.empty()) {
            std::cerr << "Failed to capture frame" << std::endl;
            break;
        }

        // Display the frame
        cv::imshow("Camera", frame);

        // Check if the 's' key was pressed
        int key = cv::waitKey(1);
        if(key == 's') {
            // Save the frame to a file
            std::string filename = "/home/fares/rbd/projects/aruco_swarm/arucoSwarm/vid/" + std::to_string(counter) + ".jpg";
            cv::imwrite(filename, frame);
            std::cout << "Saved frame to " << filename << std::endl;
            counter ++;

        }
        else if(key == 27) { // Check if the ESC key was pressed
            break;
        }
    }

    // Release the video capture device and destroy the window
    cap.release();
    cv::destroyAllWindows();
}

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