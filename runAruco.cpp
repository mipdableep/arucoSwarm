#include "include/runAruco.h"

using namespace std::chrono_literals;

// declare vars
std::string command;

void webcamTest(aruco& detector, arucoCalc& calc){

    sleep(1);

    //print target
    calc.get_target_vals();

    sleep(4);

    int tmpId = -1;
    bool commandFlag;

    while (true) {
        while (commandFlag && detector.ID != -1);

        if (detector.ID != -1) {
            tmpId = detector.ID;
        }

        if (detector.ID == -1 && tmpId != -1) {
            // noLeaderLoop_v2(drone, detector, tello, tmpId);
            std::cout<<"no leader" << std::endl;
        }

        else {
            if (!detector.init || detector.ID != -1) {

                calc.droneZRotate = detector.yaw;
                calc.droneXPos = detector.rightLeft;
                calc.droneYPos = detector.forward;
                calc.droneZPos = detector.upDown;

                // run rc calculations
                X_rc = calc.calculate_x_rc();
                Y_rc = calc.calculate_y_rc();
                Z_rc = calc.calculate_z_rc();
                Zr_rc = calc.calculate_z_rotation_rc();


                std::string command = "rc ";

                command += std::to_string((int)(X_rc));
                command += " ";

                command += std::to_string((int)(Y_rc));
                command += " ";

                command += std::to_string((int)(Z_rc));
                command += " ";

                command += std::to_string((int)(Zr_rc));

                // tello.SendCommand(command);
                std::cout << "OON command:  " << command << std::endl;

            } else {
                // tello.SendCommand("rc 0 0 0 0");
                printf("rc 0 0 0 0");
            }

            sleep(1);
            commandFlag = false;
        }
    }
}

void objectOrientedNavigation(aruco& detector, ctello::Tello& tello, arucoCalc& calc) {
    std::cout << "started OON\n" << std::endl;

    while (true) {

        if ((!detector.init || detector.ID != -1) && detector.rightId != -9) {

            calc.droneZRotate = detector.yaw;
            calc.droneXPos = detector.rightLeft;
            calc.droneYPos = detector.forward;
            calc.droneZPos = detector.upDown;

            // std::cout << "droneZRotate: " << calc.droneZRotate << std::endl;
            // std::cout << "droneXPos: " << calc.droneXPos << std::endl;
            // std::cout << "droneYPos: " << calc.droneYPos << std::endl;
            // std::cout << "droneZPos: " << calc.droneZPos << std::endl;

            // run rc calculations
            X_rc = calc.calculate_x_rc();
            Y_rc = calc.calculate_y_rc();
            Z_rc = calc.calculate_z_rc();
            Zr_rc = calc.calculate_z_rotation_rc();


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
    }
}

/*
void objectOrientedNavigation(aruco& detector, SerialTello& tello, arucoCalc& calc) {
    std::cout << "started OON\n" << std::endl;

    while (true) {

        if ((!detector.init || detector.ID != -1) && detector.rightId != -9) {

            calc.set_temp_vals();

            calc.droneZRotate = detector.yaw;
            calc.droneXPos = detector.rightLeft;
            calc.droneYPos = detector.forward;
            calc.droneZPos = detector.upDown;

            // std::cout << "droneZRotate: " << calc.droneZRotate << std::endl;
            // std::cout << "droneXPos: " << calc.droneXPos << std::endl;
            // std::cout << "droneYPos: " << calc.droneYPos << std::endl;
            // std::cout << "droneZPos: " << calc.droneZPos << std::endl;

            // run rc calculations
            X_rc = calc.calculate_x_rc();
            Y_rc = calc.calculate_y_rc();
            Z_rc = calc.calculate_z_rc();
            Zr_rc = calc.calculate_z_rotation_rc();


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
    }
}
*/

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

    int cam_fps = data["cam_fps"];

    float currentMarkerSize = data["currentMarkerSize"];

    std::string droneName = data2["DroneName"];
    bool isLeader = data2["isLeader"];

    //OON target location:
    int OON_target_X = data2["OON_target_X"];
    int OON_target_Y = data2["OON_target_Y"];
    int OON_target_Z = data2["OON_target_Z"];

    bool runServer = data["runServer"];
    bool videoCap = data["videoCap"];
    bool do_imshow = data["imshow"];
    bool connect_to_drone = data["connect_to_drone"];

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
        detector.imshow = true;
        detector.videoCap = false;
        detector.id_to_follow = data2["OON_target_id"];
        arucoCalc calc(OON_target_X, OON_target_Y, OON_target_Z);
        std::thread movementThread([&] { webcamTest(detector, calc); });
        movementThread.join();
    }
/*
    else if (rpiCamera){
        if (runServer){
            DroneClient client(droneName, argv[1], std::stoi(argv[2]));
            client.connect_to_server();
            client.wait_for_takeoff();
        }

        SerialTello tello("");

        sleep(2);

        tello.SendCommandWithResponse("takeoff");

        tello.SendCommand("rc 0 0 0 0");

        arucoCalc calc(OON_target_X, OON_target_Y, OON_target_Z);
        //camera at port 0
        aruco detector(yamlCalibrationPath, 0, currentMarkerSize, cam_fps);
        detector.imshow = false;
        detector.videoCap = videoCap;
        detector.id_to_follow = data2["OON_target_id"];
        std::thread movementThread([&] {objectOrientedNavigation(detector, tello, calc);
        });

        movementThread.join();
    }
*/

    else { //regular run with wifi connection

        if (runServer){
            DroneClient client(droneName, argv[1], std::stoi(argv[2]));
            client.connect_to_server();
            client.wait_for_takeoff();
            change_to_tello_wifi();
        }
        
        if (!runServer && connect_to_drone)
            change_to_tello_wifi();

        ctello::Tello tello;
        tello.SendCommandWithResponse("streamon");

        sleep(2);

        tello.SendCommandWithResponse("takeoff");

        tello.SendCommand("rc 0 0 0 0");
        std::string cameraString = data["cameraString"];

        arucoCalc calc(OON_target_X, OON_target_Y, OON_target_Z);
        aruco detector(yamlCalibrationPath, cameraString, currentMarkerSize);
        detector.imshow = do_imshow;
        detector.videoCap = videoCap;
        detector.id_to_follow = data2["OON_target_id"];
        std::thread movementThread([&] {objectOrientedNavigation(detector, tello, calc);
        });

        movementThread.join();
    }
}
