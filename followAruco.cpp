#include "include/followAruco.h"

#include <boost/lockfree/spsc_queue.hpp>
#include <string>
#include <thread>
#include <vector>

// #include "detector.hpp"
#include "drone_client.hpp"
#include "include/constants.h"

using namespace std::chrono_literals;

// declare vars
bool exitLoop = false;

// declare commands
std::string standStill = "rc 0 0 0 0";
std::string forward;
std::string turn360 = "rc 0 0 0 40";  // sleep for 14.5 sec
const std::string wpa_supplicant_tello_file_path =
    "/etc/wpa_supplicant/tello.conf";

void webcamTest(aruco& detector) {
    std::cout<< "in: webcamTest()" << std::endl;
    int tmpId = -1;
    bool commandFlag;

    /*obj detector
     boost::lockfree::spsc_queue<std::vector<int>>& classes_queue =
        object_detector.get_classes_queue(); */

    while (true) {
        if (!detector.init || detector.ID != -1) {
        } else {
        }

        commandFlag = false;
    }
    sleep(1);
}

void followAruco(aruco& detector, ctello::Tello& tello, int ArucoFront, int ArucoBack, int ArucoTarget/* , Detector& object_detector, int detect_class */) {

    std::cout<< "in: followAruco()" << std::endl;

    bool isFirst = false;
    if (ArucoFront == -1) isFirst = true;
    
    tello.SendCommand(standStill);
    sleep(1);
    tello.SendCommand(standStill);

    if (isFirst)
        scan360(detector, ArucoBack, tello);
    else{
        scan360(detector, ArucoFront, tello);
    }


    std::thread detectAruco(
        [&] { searchArucoTargetThread(tello, detector, ArucoTarget); });
 
    while (!exitLoop) {
        tello.SendCommand(forward);
        sleep(4);

        scanForward(detector, ArucoFront, tello);

        scan360(detector, ArucoBack, tello);

        scanForward(detector, ArucoFront, tello);
    }
}

void searchArucoTargetThread(ctello::Tello& tello, aruco& detector, int ArucoTarget){
    std::cout<< "in: searchArucoTargetThread()" << std::endl;
    
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
    std::cout<< "in: change_to_tello_wifi()" << std::endl;
    const std::string kill_connection_cmd = "sudo killall wpa_supplicant";
    const std::string connection_cmd =
        "sudo wpa_supplicant -i wlan0 -B -c " + wpa_supplicant_tello_file_path;

    std::system(kill_connection_cmd.c_str());
    std::this_thread::sleep_for(2s);
    std::system(connection_cmd.c_str());
    std::this_thread::sleep_for(10s);
}

void scan360(aruco& detector, int arucoId, ctello::Tello& tello){
    std::cout<< "in: scan360()" << std::endl;
    std::cout << "serching for aruco " << arucoId << std::endl;

    bool runDetection = true;
    bool canContinue;
    std::thread detectAruco(
        [&] { ScanForAruco(detector, arucoId, runDetection, canContinue); });

    for (int i = 0; i<6; i++){
        tello.SendCommand("cw 60");
        usleep(3500000);
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

void scanForward(aruco& detector, int arucoId, ctello::Tello& tello){
    std::cout<< "in: scanForward()" << std::endl;
    std::cout << "serching for aruco " << arucoId << std::endl;

    bool runDetection = true;
    bool canContinue;
    std::thread detectAruco(
        [&] { ScanForAruco(detector, arucoId, runDetection, canContinue); });

    tello.SendCommand("rc 0 0 0 0");
    usleep(2000000);
    tello.SendCommand("cw 60");
    usleep(3500000);
    tello.SendCommand("ccw 120");
    usleep(3500000);
    tello.SendCommand("cw 60");
    usleep(3500000);


    tello.SendCommand("rc 0 0 0 0");
    runDetection = false;
    usleep(1000000);

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

void doCommand(aruco& detector, int arucoId, ctello::Tello& tello, std::string command, float amountOfSleep) {
    std::cout<< "in: doCommand()" << std::endl;
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

void ScanForAruco(aruco& detector, int arucoId, bool& runDetection, bool& canContinue) {
    std::cout<< "in: ScanForAruco()" << std::endl;
    int counter = 0;
    while (runDetection) {
        for (int i : detector.ids) {
            if (i == arucoId) counter++;
            std::cout<<"++ ";
        }
        usleep(100000);
    }
    std::cout << std::endl << "aruco counter = " << counter << std::endl;

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
    int dist_forward = data["dist_forward"];
    forward = "forward " + std::to_string(dist_forward);
    bool runServer = data["runServer"];
    bool imshowStream = data["imshow"];


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
        detector.imshowStream = imshowStream;
        
        // Detector object_detector(argv[1], detector.get_frame_queue());
        std::thread movementThread([&] {
            followAruco(detector, tello, ArucoFront, Arucoback, ArucoTarget/* , object_detector,
                        detect_class */);
        });

        movementThread.join();
    }
}
