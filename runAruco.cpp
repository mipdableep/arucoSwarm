#include "include/runAruco.h"

using namespace std::chrono_literals;

// declare vars
std::string command;

void webcamTest(aruco_utils &detector, arucoCalc &calc)
{

    sleep(1);

    // print target
    calc.get_target_vals();

    sleep(4);

    int tmpId = -1;
    bool commandFlag;

    std::cout << "started OON\n"
              << std::endl;
    calc.get_target_vals();

    int loop_counter = 0;

    while (run_OON)
    {

        if (detector.correct_index == -9 || !detector.Target_found)
        {
            usleep(1000000);
            loop_counter++;
            continue;
        }

        calc.droneZRotate = detector.yaw;
        calc.droneXPos = detector.Tx;
        calc.droneYPos = detector.Ty;
        calc.droneZPos = detector.Tz;

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

        std::cout << "OON command:  " << command << std::endl;

        usleep(1000000);
        loop_counter++;
    }
}

void objectOrientedNavigation(aruco_utils &detector, ctello::Tello &tello, arucoCalc &calc)
{
    std::cout << "started OON\n"
              << std::endl;
    calc.get_target_vals();

    int loop_counter = 0;

    while (run_OON)
    {

        if (detector.correct_index == -9 || !detector.Target_found)
        {
            tello.SendCommand("rc 0 0 0 0");
            usleep(1000000);
            loop_counter++;
            continue;
        }

        calc.droneZRotate = detector.yaw;
        calc.droneXPos = detector.Tx;
        calc.droneYPos = detector.Ty;
        calc.droneZPos = detector.Tz;

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

        usleep(1000000);
        loop_counter++;
    }

    tello.SendCommand("rc 0 0 0 0");
    usleep(200000);
    tello.SendCommand("rc 0 0 0 0");
}

void change_to_tello_wifi(const std::string tello_conf_path)
{

    std::cout << "in: change_to_tello_wifi()" << std::endl;

    const std::string kill_connection_cmd = "sudo killall wpa_supplicant";
    const std::string connection_cmd =
        "sudo wpa_supplicant -i wlan0 -B -c " + tello_conf_path;

    std::system(kill_connection_cmd.c_str());
    std::this_thread::sleep_for(2s);
    std::system(connection_cmd.c_str());
    std::this_thread::sleep_for(10s);
}

void timer_limiter(aruco_utils &detector, ctello::Tello &tello, bool &run_OON, int seconds_time_amount)
{
    std::cout <<"runtime len: "<<seconds_time_amount<<std::endl;
    if (seconds_time_amount == -1){
        while (detector.exit != 'q')
            sleep(1);
    }

    else{
    while (seconds_time_amount > 0)
        {
            sleep(1);
            seconds_time_amount--;
            if (seconds_time_amount % 5 == 0)
                std::cout << "time left: " << seconds_time_amount << std::endl;
            
            // if q pressed on imshow
            if (detector.exit == 'q')
                seconds_time_amount = -1;
        }
    }
    run_OON = false;
    detector.~aruco_utils();
    usleep(300000);
    tello.SendCommand("land");
}

int main(int argc, char *argv[])
{

    std::ifstream programGlobal("../config.json");
    std::ifstream programDrone("../drone_config.json");

    nlohmann::json global_vars;
    programGlobal >> global_vars;
    programGlobal.close();

    nlohmann::json drone_vars;
    programDrone >> drone_vars;
    programDrone.close();

    //* drone number - for drone-config
    std::string DRONE_NUM = global_vars["DRONE_NUM"];

    int runtime_length = global_vars["runtime_length"];

    int cam_fps = global_vars["cam_fps"];
    bool isWebcam = global_vars["webcam"];
    bool rpiCamera = global_vars["rpiCamera"];

    bool save_video = global_vars["save_video"];
    bool do_imshow = global_vars["imshow"];
    std::string save_video_path = global_vars["save_video_path"];

    bool runServer = global_vars["runServer"];
    bool connect_to_drone = global_vars["connect_to_drone"];
    bool do_ncli_command = global_vars["do_ncli_command"];

    bool isLeader = drone_vars[DRONE_NUM]["isLeader"];
    int OON_TARGET_ID = drone_vars[DRONE_NUM]["OON_target_id"];
    std::string droneName = drone_vars[DRONE_NUM]["DroneName"];
    float currentMarkerSize = drone_vars[DRONE_NUM]["currentMarkerSize"];

    const std::string tello_conf_path = drone_vars[DRONE_NUM]["tello_conf_path"];

    // OON target location:
    int OON_target_X = drone_vars[DRONE_NUM]["OON_target_X"];
    int OON_target_Y = drone_vars[DRONE_NUM]["OON_target_Y"];
    int OON_target_Z = drone_vars[DRONE_NUM]["OON_target_Z"];

    // checking the img input device for correct calibration
    std::string yamlCalibrationPath;

    if (isWebcam)
    {
        yamlCalibrationPath = global_vars["webcamYamlCalibrationPath"];
    }
    else if (rpiCamera)
    {
        yamlCalibrationPath = global_vars["rpiCalibrationPath"];
    }
    else
    {
        yamlCalibrationPath = global_vars["yamlCalibrationPath"];
    }

    if (isWebcam)
    {
        int cameraPort = global_vars["cameraPort"];
        aruco_utils detector(yamlCalibrationPath, cameraPort, currentMarkerSize);
        detector.imshow = true;
        detector.save_video = false;
        detector.id_to_follow = OON_TARGET_ID;
        arucoCalc calc(OON_target_X, OON_target_Y, OON_target_Z);

        std::thread markerThread([&]
                                 { detector.open_video_cap(cameraPort,0,0); });
        std::thread movementThread([&]
                                   { webcamTest(detector, calc); });
        movementThread.join();
        markerThread.join();
    }

    else
    { // regular run with wifi connection

        if (runServer)
        {
            int serverPort = global_vars["serverPort"];
            std::string serverHostIp = global_vars["serverHostIp"];

            DroneClient client(droneName, serverHostIp, serverPort);
            client.connect_to_server();
            client.wait_for_takeoff();
            change_to_tello_wifi(tello_conf_path);
        }

        if (!runServer && connect_to_drone)
            change_to_tello_wifi(tello_conf_path);

        droneName = "TELLO-98F041";

        if (do_ncli_command)
        {
            std::string commandString = "nmcli c up " + droneName;
            const char *command = commandString.c_str();
            system(command);
        }

        ctello::Tello tello;
        tello.SendCommandWithResponse("streamon");

        sleep(2);

        // tello.SendCommandWithResponse("takeoff");

        tello.SendCommand("rc 0 0 0 0");
        std::string cameraString = global_vars["cameraString"];

        arucoCalc calc(OON_target_X, OON_target_Y, OON_target_Z);
        aruco_utils detector(yamlCalibrationPath, currentMarkerSize, OON_TARGET_ID);
        detector.imshow = do_imshow;
        detector.save_video = save_video;
        detector.save_run_path = save_video_path;
        detector.id_to_follow = OON_TARGET_ID;

        std::thread videoThread([&]
                                { detector.open_video_cap(cameraString,0,0); });
        std::thread markerThread([&]
                                 { detector.main_aruco_loop(); });
        std::thread movementThread([&]
                                   { objectOrientedNavigation(detector, tello, calc); });
        std::thread countdownThread([&]
                                    { timer_limiter(detector, tello, run_OON, runtime_length); });
        videoThread.join();
        markerThread.join();
        movementThread.join();
        countdownThread.join();

        if (do_ncli_command)
        {
            std::string commandString = "nmcli c up rpi-hotspot";
            const char *command = commandString.c_str();
            system(command);
        }

        // if exit or return 0 is in the first scope it outputs "std::system_error - what():  Invalid argument"
        exit(0);
    }
}