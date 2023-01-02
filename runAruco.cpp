#include "include/runAruco.h"

#include <boost/lockfree/spsc_queue.hpp>
#include <vector>

#include "include/constants.h"

using namespace std::chrono_literals;

// declare vars
double droneZRotate, droneXPos, droneYPos, droneZPos;
double current_wanted_Zr;
int X_rc, Y_rc, Z_rc, Zr_rc;
int noLeaderCycle = 0;
std::string command;

void webcamTest(aruco& detector){}

void leaderDrone(ctello::Tello& tello) {
    sleep(2);
    for (int i = 0; i < 10; i++) {
        tello.SendCommand("rc 0 0 0 0");
        sleep(3);
    }

    while (true) {
        tello.SendCommand("rc 0 15 0 0");
        sleep(10);
        tello.SendCommand("rc 0 0 0 0");
        sleep(1);
        tello.SendCommand("rc 0 0 0 10");
        sleep(4);
        tello.SendCommand("rc 0 0 0 0");
        sleep(5);
        tello.SendCommand("rc 0 0 0 -10");
        sleep(8);
        tello.SendCommand("rc 0 0 0 0");
        sleep(5);
        tello.SendCommand("rc 0 0 0 10");
        sleep(4);
        tello.SendCommand("rc 0 0 0 0");
        sleep(1);
    }
}

void objectOrientedNavigation(aruco& detector, ctello::Tello& tello) {
    std::cout << "started OON\n" << std::endl;

    int tmpId = -1;
    bool commandFlag;
    // check land vriables
    int rc_y_below_land_limit;
    int wentDownCounter = 0;

    while (true) {
        while (commandFlag && detector.ID != -1)
            ;

        if (detector.ID != -1) {
            tmpId = detector.ID;
        }

        if (detector.ID == -1 && tmpId != -1) {
            // noLeaderLoop_v2(drone, detector, tello, tmpId);
            noLeaderLoop(detector, tello, tmpId);
        }

        else {
            if (!detector.init || detector.ID != -1) {

                double tmpZr = droneZRotate, tmpZ = droneZPos, tmpX = droneXPos;

                bool made_switch_last_time;

                droneZRotate = detector.yaw;
                droneXPos = detector.rightLeft;
                droneYPos = detector.forward;
                droneZPos = detector.upDown;

                bool opposite_droneZr, opposite_droneZ, opposite_droneX;

                opposite_droneZr = opposite_position(droneZRotate, tmpZr);
                opposite_droneZ = opposite_position(droneZPos, tmpZ);
                opposite_droneX = opposite_position(droneXPos, tmpX);

                if (opposite_droneZr && opposite_droneZ && opposite_droneX) {
                    droneXPos *= -1;
                    droneZPos *= -1;
                    droneZRotate *= -1;
                    std::cout << "make switch\n" << std::endl;
                }

                // run rc calculations
                calculate_x_rc();
                calculate_y_rc();
                calculate_z_rc();
                calculate_z_rotation_rc();


                std::string command = "rc ";

                command += std::to_string((int)(X_rc));
                command += " ";

                command += std::to_string((int)(Y_rc));
                command += " ";

                command += std::to_string((int)(Z_rc));
                command += " ";

                command += std::to_string((int)(Zr_rc));

                tello.SendCommand(command);
                std::cout << "OON command:  " << command << std::endl;

            } else {
                tello.SendCommand("rc 0 0 0 0");
            }

            sleep(1);
            commandFlag = false;
        }
    }
}

void noLeaderLoop(aruco& detector, ctello::Tello& tello,
                  int& tmpId) {
    std::cout << "in no leader loop" << std::endl;
    int sleepAmount = 2;
    // wait to see if problem solevs itself
    tello.SendCommand("rc 0 0 0 0");
    printf("sleeping: %d seconds", sleepAmount);
    sleep(sleepAmount);

    int i = 0;
    while (detector.ID == -1 && i < 50) {
        tello.SendCommand("rc 0 0 0 0");
        std::cout << "Searching for leader" << std::endl;
        tello.SendCommand("rc 0 0 0 25");
        i++;
        if (detector.ID != tmpId && detector.ID != -1) detector.init = true;
        sleep(2);
    }

    if (detector.ID == -1) tello.SendCommandWithResponse("land");
}

void calculate_y_rc() {
    // if current > target + tollorate
    // if bigger then rc limit
    if (droneYPos > Y_TARGET + Y_DIST_TOLORANCE) {
        if ((droneYPos - Y_TARGET) / 3 > Y_LIMIT_RC) {
            Y_rc = -Y_LIMIT_RC;
            // std::cout<<1<<std::endl;
        } else {
            Y_rc = -((droneYPos - Y_TARGET) / 3);
            // std::cout<<2<<std::endl;
        }
    } else {
        Y_rc = 0;
    }

    if (droneYPos < Y_TARGET - Y_DIST_TOLORANCE) {
        if ((Y_TARGET - droneYPos) / 3 > Y_LIMIT_RC) {
            Y_rc = Y_LIMIT_RC;
            // std::cout<<3<<std::endl;
        } else {
            Y_rc = (Y_TARGET - droneYPos) / 3;
            // std::cout<<4<<std::endl;
        }
    }

    Y_rc *= -1;
}

void calculate_z_rc() {
    // if current > target + tollorate
    // if bigger then rc limit
    if (droneZPos > Z_TARGET + Z_DIST_TOLORANCE) {
        if ((droneZPos - Z_TARGET) / 2 > Z_LIMIT_RC)
            Z_rc = -Z_LIMIT_RC;
        else
            Z_rc = -((droneZPos - Z_TARGET) / 2);
    } else {
        Z_rc = 0;
    }

    if (droneZPos < Z_TARGET - Z_DIST_TOLORANCE) {
        if ((Z_TARGET - droneZPos) / 2 > Z_LIMIT_RC)
            Z_rc = Z_LIMIT_RC;
        else
            Z_rc = (Z_TARGET - droneZPos) / 2;
    }
}


void calculate_current_wanted_Zr() {
    current_wanted_Zr = std::atan2(droneXPos, droneYPos) * RADIANS_TO_DEGREESE;
}

void calculate_z_rotation_rc() {
    calculate_current_wanted_Zr();
    Zr_rc = (droneZRotate - current_wanted_Zr)*1.15;
}

void calculate_x_rc() { X_rc = (Z_ANGLE_TARGET - current_wanted_Zr) / 2; }

bool opposite_position(double droneVal, double tmp) {
    return (tmp - (0.4 * tmp) < -droneVal && -droneVal < tmp + (0.4 * tmp));
}

bool opposite_angle(double droneVal, double tmp) {
    return (tmp - (0.7 * tmp) < -droneVal && -droneVal < tmp + (0.7 * tmp));
}

void change_to_tello_wifi() {
    const std::string wpa_supplicant_tello_file_path =
    "/etc/wpa_supplicant/tello.conf";
    std::cout<< "in: change_to_tello_wifi()" << std::endl;
    const std::string kill_connection_cmd = "sudo killall wpa_supplicant";
    const std::string connection_cmd =
        "sudo wpa_supplicant -i wlan0 -B -c " + wpa_supplicant_tello_file_path;

    std::system(kill_connection_cmd.c_str());
    std::this_thread::sleep_for(2s);
    std::system(connection_cmd.c_str());
    std::this_thread::sleep_for(10s);
}

int main(int argc, char* argv[]) {

    std::ifstream programData("../config.json");
    std::ifstream programData2("../drone_config.json");

    nlohmann::json data;
    programData >> data;
    programData.close();
    
    nlohmann::json data2;
    programData2 >> data2;
    programData2.close();

    bool isWebcam = data["webcam"];
    bool rpiCamera = data["rpiCamera"];

    float currentMarkerSize = data["currentMarkerSize"];

    std::string droneName = data2["DroneName"];
    bool isLeader = data2["isLeader"];

    //OON target location:
    int OON_target_X = data2["OON_target_X"];
    int OON_target_Y = data2["OON_target_Y"];
    int OON_target_Z = data2["OON_target_Z"];
    int OON_target_Zr = data2["OON_target_Zr"];


    bool runServer = data["runServer"];
    

    // checking the img input device for correct calibration
    std::string yamlCalibrationPath;

    if (isWebcam) {
        yamlCalibrationPath = data["webcamYamlCalibrationPath"];
    } else if (rpiCamera) {
        yamlCalibrationPath = data["rpiCalibrationPath"];
    }else{
        yamlCalibrationPath = data["yamlCalibrationPath"];
    }

    if (isWebcam) {
        int cameraPort = data["cameraPort"];
        aruco detector(yamlCalibrationPath, cameraPort, currentMarkerSize);
        std::thread movementThread([&] { webcamTest(detector); });
        movementThread.join();
    }
    //TODO: addif 
    /*(runServer){
            DroneClient client(droneName, argv[2], std::stoi(argv[3]));
            client.connect_to_server();
            client.wait_for_takeoff();
            change_to_tello_wifi();
        }*/

    else {

        ctello::Tello tello;
        tello.SendCommandWithResponse("streamon");

        sleep(2);

        tello.SendCommandWithResponse("takeoff");

        tello.SendCommand("rc 0 0 0 0");
        std::string cameraString = data["cameraString"];
        aruco detector(yamlCalibrationPath, cameraString, currentMarkerSize);
        std::thread movementThread([&] {objectOrientedNavigation(detector, tello);
        });

        movementThread.join();
    }
}
