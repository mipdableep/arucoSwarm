
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
#include <nlohmann/json.hpp>

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
    std::ifstream programGlobal("config.json");

    nlohmann::json global_vars;
    programGlobal >> global_vars;
    programGlobal.close();


    //* drone number - for drone-config
    std::string DRONE_NUM = global_vars["DRONE_NUM"];

    int runtime_length = global_vars["runtime_length"];
    
    int cam_fps = global_vars["cam_fps"];
    bool isWebcam = global_vars["webcam"];
    bool rpiCamera = global_vars["rpiCamera"];
    
    bool save_video = global_vars["videoCap"];
    bool do_imshow = global_vars["imshow"];

    bool runServer = global_vars["runServer"];
    bool connect_to_drone = global_vars["connect_to_drone"];
    bool do_ncli_command = global_vars["do_ncli_command"];
    
    // std::ifstream programDrone("../drone_config.json");
    // nlohmann::json drone_vars;
    // programDrone >> drone_vars;
    // programDrone.close();
    // drone_vars = drone_vars[DRONE_NUM];

    // bool isLeader = drone_vars["isLeader"];
    // std::cout << "isLeader " << isLeader << std::endl;

    // int OON_TARGET_ID = drone_vars["OON_target_id"];
    // std::cout << "OON_TARGET_ID " << OON_TARGET_ID << std::endl;

    // std::string droneName = drone_vars["DroneName"];
    // std::cout << "droneName " << droneName << std::endl;

    // float currentMarkerSize = drone_vars["currentMarkerSize"];
    // std::cout << "currentMarkerSize " << currentMarkerSize << std::endl;


    // const std::string tello_conf_path = drone_vars["tello_conf_path"];
    // std::cout << "tello_conf_path " << tello_conf_path << std::endl;


    // //OON target location:
    // int OON_target_X = drone_vars["OON_target_X"];
    // std::cout << "OON_target_X " << OON_target_X << std::endl;

    // int OON_target_Y = drone_vars["OON_target_Y"];
    // std::cout << "OON_target_Y " << OON_target_Y << std::endl;

    // int OON_target_Z = drone_vars["OON_target_Z"];
    // std::cout << "OON_target_Z " << OON_target_Z << std::endl;


    // // checking the img input device for correct calibration
    // std::string yamlCalibrationPath;

    // if (isWebcam) {
    //     yamlCalibrationPath = global_vars["webcamYamlCalibrationPath"];
    // } else if (rpiCamera) {
    //     yamlCalibrationPath = global_vars["rpiCalibrationPath"];
    // }else{
    //     yamlCalibrationPath = global_vars["yamlCalibrationPath"];
    // }

}