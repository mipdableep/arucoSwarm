#include "include/runAruco.h"

using namespace std::chrono_literals;

// declare vars
std::string command;

void webcamTest(aruco_utils &detector, arucoCalc &calc)
{
    std::cout << "started OON\n"
              << std::endl;
    calc.get_target_vals();

    int loop_counter = 0;

    while (run_OON)
    {

        // if the correct index was not found print and go back
        if (detector.correct_index == -9 || !detector.Target_found)
        {
            std::cout<<"not found"<<std::endl;
            usleep(500000);
            loop_counter++;
            continue;
        }


        calc.droneZRotate = detector.yaw;
        calc.droneXPos = detector.Tx;
        calc.droneYPos = detector.Ty;
        calc.droneZPos = detector.Tz;

        std::cout<< "yaw:  " << detector.yaw << std::endl;
        std::cout<< "Tx:  " << detector.Tx << std::endl;
        std::cout<< "Ty:  " << detector.Ty << std::endl;
        std::cout<< "Tz:  " << detector.Tz << std::endl;

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
        while (!detector.exit)
            sleep(1);
        exit_all(detector, run_OON, tello);
        return;
    }


    while (seconds_time_amount > 0)
    {
        sleep(1);
        seconds_time_amount--;
        if (seconds_time_amount % 5 == 0)
            std::cout << "time left: " << seconds_time_amount << std::endl;
        
        // if q pressed on imshow
        if (detector.exit)
            seconds_time_amount = 0;
    }

    exit_all(detector, run_OON, tello);
}

void exit_all(aruco_utils& detector, bool& run_OON, ctello::Tello& tello)
{
    run_OON = false;
    detector.close();
    tello.SendCommand("land");
}

void exit_all(aruco_utils& detector, bool& run_OON)
{
    run_OON = false;
    detector.close();
}

int main(int argc, char *argv[])
{

    std::ifstream config("../config.json");

    nlohmann::json conf;
    config >> conf;
    config.close();

    // global conf str
    std::string G = "global-config";

    //* drone number - for drone-config
    std::string DEVICE = conf[G]["DEVICE"];

    // run settings
    int  runtime_length = conf[G]["runtime_length"];
    int  rc_devidor     = conf[G]["rc_devidor"];
    bool send_takeoff   = conf[G]["send_takeoff"];

    // capture settings
    bool isWebcam  = DEVICE == "webcam";
    int cameraPort = conf[G]["cameraPort"];

    std::string cameraString = conf[G]["cameraString"];

    // video settings
    bool do_imshow  = conf[G]["imshow"];
    bool save_video = conf[G]["save_video"];
    std::string save_video_path = conf[G]["save_video_path"];

    // connection settings
    bool runServer           = conf[G]["runServer"];
    int serverPort           = conf[G]["serverPort"];
    std::string serverHostIp = conf[G]["serverHostIp"];
    bool connect_to_drone    = conf[G]["connect_to_drone"];
    bool do_ncli_command     = conf[G]["do_ncli_command"];

    // calibration settings
    std::string yamlCalibrationPath = conf[DEVICE]["calibration_path"];


    bool isLeader           = conf[DEVICE]["isLeader"];
    int OON_TARGET_ID       = conf[DEVICE]["OON_target_id"];
    std::string droneName   = conf[DEVICE]["DroneName"];
    float currentMarkerSize = conf[DEVICE]["currentMarkerSize"];

    const std::string tello_conf_path = conf[DEVICE]["tello_conf_path"];

    // OON target location:
    int OON_target_X        = conf[DEVICE]["OON_target_X"];
    int OON_target_Y        = conf[DEVICE]["OON_target_Y"];
    int OON_target_Z        = conf[DEVICE]["OON_target_Z"];


    

    if (isWebcam)
    {
        aruco_utils detector(yamlCalibrationPath, currentMarkerSize, cameraPort);
        detector.imshow = true;
        detector.save_video = false;
        detector.id_to_follow = OON_TARGET_ID;
        
        arucoCalc calc(OON_target_X, OON_target_Y, OON_target_Z);
        calc.setDevidors(rc_devidor);


        std::thread videoThread   ([&]{ detector.open_video_cap(cameraPort,0,0); });
        std::thread markerThread  ([&]{ detector.main_aruco_loop(); });
        std::thread movementThread([&]{ webcamTest(detector, calc); });

        while (!detector.exit)
            sleep(1);
        exit_all(detector, run_OON);
        sleep(1);

        videoThread.join();
        markerThread.join();
        movementThread.join();

    }

    else
    { // regular run with wifi connection

        if (runServer)
        {

            DroneClient client(droneName, serverHostIp, serverPort);
            client.connect_to_server();
            client.wait_for_takeoff();
            change_to_tello_wifi(tello_conf_path);
        }

        if (!runServer && connect_to_drone)
            change_to_tello_wifi(tello_conf_path);


        if (do_ncli_command)
        {
            // ! REMOVE!
            droneName = "TELLO-98F041";

            std::string commandString = "nmcli c up " + droneName;
            const char *command = commandString.c_str();
            system(command);
        }

        ctello::Tello tello;
        tello.SendCommandWithResponse("streamon");

        sleep(2);

        if (send_takeoff)
            tello.SendCommandWithResponse("takeoff");

        tello.SendCommand("rc 0 0 0 0");

        aruco_utils detector(yamlCalibrationPath, currentMarkerSize, OON_TARGET_ID);
        detector.imshow = do_imshow;
        detector.save_video = save_video;
        detector.save_run_path = save_video_path;
        detector.id_to_follow = OON_TARGET_ID;

        arucoCalc calc(OON_target_X, OON_target_Y, OON_target_Z);
        calc.setDevidors(rc_devidor);

        std::thread markerThread   ([&]{ detector.main_aruco_loop(); });
        std::thread videoThread    ([&]{ detector.open_video_cap(cameraString); });

        std::thread movementThread ([&]{ objectOrientedNavigation(detector, tello, calc); });
        std::thread countdownThread([&]{ timer_limiter(detector, tello, run_OON, runtime_length); });
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