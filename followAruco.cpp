#include "include/followAruco.h"

#include <boost/lockfree/spsc_queue.hpp>
#include <vector>
#include "include/constants.h"

// declare vars
double droneZRotate, droneXPos, droneYPos, droneZPos;
double current_wanted_Zr;
int X_rc, Y_rc, Z_rc, Zr_rc;
int noLeaderCycle = 0;
std::string command;

void webcamTest(aruco& detector) {
    
    int tmpId = -1;
    bool commandFlag;
    
    /*obj detector
     boost::lockfree::spsc_queue<std::vector<int>>& classes_queue =
        object_detector.get_classes_queue(); */

    while (true) {
        
        while (commandFlag && detector.ID != -1)
            ;

        if (detector.ID != -1) {
            tmpId = detector.ID;
        }

        if (detector.ID == -1 && tmpId != -1) {
            // noLeaderLoop(drone, detector, tello, tmpId, sleepAmount);
            printf("would do noLeader\n");
        }

        else {
            /* obj detector
             if (!classes_queue.empty()) {
                std::vector<int> classes_in_frame;
                classes_queue.pop(classes_in_frame);

                if (std::find(classes_in_frame.begin(), classes_in_frame.end(),
                              1) != classes_in_frame.end()) {
                    std::cout << "\n\n object detected by model!\n"
                              << std::endl;
                }
            } */

            if (!detector.init || detector.ID != -1) {
                
                
            } else {
                std::cout << "in else" << std::endl;
            }

            commandFlag = false;
        }
        sleep(1);
    }
}

int main(int argc, char* argv[]) {
    std::ifstream programData("../config.json");

    nlohmann::json data;
    programData >> data;
    programData.close();
    bool isWebcam = data["webcam"];
    std::string droneName = data["DroneName"];
    float currentMarkerSize = data["currentMarkerSize"];
    bool isLeader = data["isLeader"];
    std::string commandString = "nmcli c up " + droneName;
    const char* command = commandString.c_str();

    // checking the img input device for correct calibration
    std::string yamlCalibrationPath;

    if (isWebcam) {
        yamlCalibrationPath = data["webcamYamlCalibrationPath"];
    } else {
        yamlCalibrationPath = data["yamlCalibrationPath"];
    }

    if (isWebcam) {
        int cameraPort = data["cameraPort"];
        aruco detector(yamlCalibrationPath, cameraPort, currentMarkerSize);
        std::thread movementThread(
            [&] { webcamTest(detector); });
        movementThread.join();
    }

    else {
        /* drone d1;
        std::string commandString = "nmcli c up " + droneName;
        system(command);
        ctello::Tello tello;
        tello.SendCommandWithResponse("streamon");

        sleep(2);

        tello.SendCommandWithResponse("takeoff");

        tello.SendCommand("rc 0 0 0 0");
        std::string cameraString = data["cameraString"];
        aruco detector(yamlCalibrationPath, cameraString, currentMarkerSize);
        Detector object_detector(argv[1], detector.get_frame_queue());
        std::thread movementThread([&] {
            objectOrientedNavigation(d1, detector, tello, object_detector);
        });
        // std::thread movementThread([&] { webcamTest(detector); } );

        movementThread.join(); */
        printf("in drone, wrong place");
    }
}
