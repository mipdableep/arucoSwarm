#include "include/followAruco.h"

#include <boost/lockfree/spsc_queue.hpp>
#include <string>
#include <thread>
#include <vector>

#include "detector.hpp"
#include "drone_client.hpp"
#include "include/constants.h"

using namespace std::chrono_literals;

// declare vars
bool exitLoop = false;

// removed for rpi
/* std::string networkString = "nmcli c up HuAirPort";
const char* commandEnd = networkString.c_str();
 */

// declare commands
std::string standStill = "rc 0 0 0 0";
std::string forward = "rc 0 25 0 0";
std::string turn360 = "rc 0 0 0 40";  // sleep for 14.5 sec
const std::string wpa_supplicant_tello_file_path =
    "/etc/wpa_supplicant/tello.conf";

void webcamTest(aruco& detector) {
    int tmpId = -1;
    bool commandFlag;

    /*obj detector
     boost::lockfree::spsc_queue<std::vector<int>>& classes_queue =
        object_detector.get_classes_queue(); */

    while (true) {
        if (!detector.init || detector.ID != -1) {
        } else {
            std::cout << "in else" << std::endl;
        }

        commandFlag = false;
    }
    sleep(1);
}

void followAruco(aruco& detector, ctello::Tello& tello, int ArucoFront,
                 int ArucoBack, int ArucoTarget/* , Detector& object_detector, int detect_class */) {
    bool isFirst = false;

    if (ArucoFront == -1) isFirst = true;
    
    tello.SendCommand(standStill);
    sleep(1);
    tello.SendCommand(standStill);

    if (isFirst)
        doCommand(detector, ArucoBack, tello, turn360, 14.5);
    else{
        doCommand(detector, ArucoFront, tello, turn360, 14.5);
    }


    std::thread detectAruco(
        [&] { searchArucoTargetThread(tello, detector, ArucoTarget); });
 
    while (!exitLoop) {
        doCommand(detector, ArucoFront, tello, standStill, 4);

        tello.SendCommand(forward);
        sleep(4);
        tello.SendCommand(standStill);

        doCommand(detector, ArucoFront, tello, standStill, 4);

        doCommand(detector, ArucoBack, tello, turn360, 14.5);
    }
}

void searchArucoTargetThread(ctello::Tello& tello, aruco& detector, int ArucoTarget){
    
    bool runDetection = true;
    bool targetDetected = false;
    int counter = 0;
    while (runDetection) {
        for (int i : detector.ids) {
            if (i == ArucoTarget) counter++;
        }
        usleep(70000);
        if (counter > 2){
            runDetection = false;
            targetDetected = true;
        }
    }

    if (targetDetected) {
        std::cout << "landing, object detected" << std::endl;
        
        tello.SendCommand("up 80");
        sleep(3);
        tello.SendCommand("rc 0 0 -80 100");
        sleep(5);
        tello.SendCommandWithResponse("land");
        exit(0);
    
    } else {
        usleep(70000);
    }
}

void change_to_tello_wifi() {
    const std::string kill_connection_cmd = "sudo killall wpa_supplicant";
    const std::string connection_cmd =
        "sudo wpa_supplicant -i wlan0 -B -c " + wpa_supplicant_tello_file_path;

    std::system(kill_connection_cmd.c_str());
    std::this_thread::sleep_for(2s);
    std::system(connection_cmd.c_str());
    std::this_thread::sleep_for(10s);
}

void doCommand(aruco& detector, int arucoId, ctello::Tello& tello,
               std::string command, float amountOfSleep) {
    int amountOfUSleep = amountOfSleep * 1000000;
    bool runDetection = true;
    bool canContinue;
    std::thread detectAruco(
        [&] { ScanForAruco(detector, arucoId, runDetection, canContinue); });

    std::cout << "serching for aruco " << arucoId << std::endl;
    if (amountOfSleep / 5 > 1) {
        int whileAmount = amountOfSleep / 5;
        while (whileAmount > 0) {
            tello.SendCommand(command);
            sleep(5);
            whileAmount--;
        }
        tello.SendCommand(command);
        usleep(amountOfUSleep % 5000000);
    } else {
        tello.SendCommand(command);
        usleep(amountOfUSleep);
    }
    tello.SendCommand("rc 0 0 0 0");
    runDetection = false;
    usleep(300000);

    detectAruco.join();

    if (!canContinue && arucoId != -1) {
        std::cout << "didnt detect aruco " << arucoId << ", landing!"
                  << std::endl;

        tello.SendCommand("rc 0 0 -80 -100");
        sleep(5);
        tello.SendCommand("land");
        exit(0);
    }
}

void ScanForAruco(aruco& detector, int arucoId, bool& runDetection,
                  bool& canContinue) {
    int counter = 0;
    while (runDetection) {
        for (int i : detector.ids) {
            if (i == arucoId) counter++;
        }
        usleep(100000);
    }
    std::cout << "aruco counter = " << counter << std::endl;

    if (counter > 5 || arucoId == -1)
        canContinue = true;
    else {
        canContinue = false;
        std::cout << "aruco wasent detected enugh" << std::endl;
    }
}

int main(int argc, char* argv[]) {

    std::ifstream programData("../config.json");

    nlohmann::json data;
    programData >> data;
    programData.close();
    bool isWebcam = data["webcam"];

    //drone name will be the name that apears on the server
    std::string droneName = data["DroneName"];
    float currentMarkerSize = data["currentMarkerSize"];
    int ArucoFront = data["ArucoIdFront"];
    int Arucoback = data["ArucoIdBehind"];
    int ArucoTarget = data["ArucoIdTarget"];
    bool runServer = data["runServer"];


    std::string yamlCalibrationPath;
    if (isWebcam) 
        yamlCalibrationPath = data["webcamYamlCalibrationPath"];
    else
        yamlCalibrationPath = data["yamlCalibrationPath"];



    if (isWebcam) {
        int cameraPort = data["cameraPort"];
        aruco detector(yamlCalibrationPath, cameraPort, currentMarkerSize);
        std::thread movementThread([&] { webcamTest(detector); });
        movementThread.join();
    }

    else {


        if (runServer){
            DroneClient client(droneName, argv[2], std::stoi(argv[3]));
            client.connect_to_server();
            client.wait_for_takeoff();
            change_to_tello_wifi();
        }

        ctello::Tello tello;

        tello.SendCommandWithResponse("streamon");

        sleep(2);

        tello.SendCommandWithResponse("takeoff");

        tello.SendCommand("rc 0 0 0 0");

        std::string cameraString = data["cameraString"];
        aruco detector(yamlCalibrationPath, cameraString, currentMarkerSize);
        // Detector object_detector(argv[1], detector.get_frame_queue());
        std::thread movementThread([&] {
            followAruco(detector, tello, ArucoFront, Arucoback, ArucoTarget/* , object_detector,
                        detect_class */);
        });

        movementThread.join();
    }
}
