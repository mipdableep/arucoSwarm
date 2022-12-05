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
                 int ArucoBack, Detector& object_detector, int detect_class) {
    bool initloop = true;
    int counter = 0;

    // TODO: add thread so first wont start before everybody
    if (ArucoFront == -1) initloop = false;
    while (initloop) {
        for (int i : detector.ids) {
            if (i == ArucoFront)
                ;
            initloop = false;
        }
        usleep(100000);
        counter++;
        if (counter > 30) {
            counter = 0;
            tello.SendCommand("rc 0 0 0 0");
        }
    }

    std::thread detectAruco(
        [&] { detectorThread(tello, object_detector, detect_class); });

    while (!exitLoop) {
        doCommand(detector, ArucoFront, tello, standStill, 4);

        tello.SendCommand(forward);
        sleep(4);
        tello.SendCommand(standStill);

        doCommand(detector, ArucoFront, tello, standStill, 4);

        doCommand(detector, ArucoBack, tello, turn360, 14.5);
    }
}

void detectorThread(ctello::Tello& tello, Detector& object_detector,
                    int detect_class) {
    object_detector.start_detection();
    boost::lockfree::spsc_queue<std::vector<int>>& classes_queue =
        object_detector.get_classes_queue();
    std::vector<int> classes_in_frame;
    while (true) {
        if (!classes_queue.empty()) {
            classes_queue.pop(classes_in_frame);

            if (std::find(classes_in_frame.begin(), classes_in_frame.end(),
                          detect_class) != classes_in_frame.end()) {
                std::cout << "landing, object detected" << std::endl;
                tello.SendCommand("stop");
                tello.SendCommandWithResponse("land");
                exit(0);
            }
        } else {
            usleep(70000);
        }
    }
}

void change_to_tello_wifi() {
    const std::string connection_cmd =
        "sudo killall wpa_supplicant && sudo "
        "wpa_supplicant -i wlan0 -B -c " +
        wpa_supplicant_tello_file_path;
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
    bool checkRc = false;

    std::ifstream programData("../config.json");

    nlohmann::json data;
    programData >> data;
    programData.close();
    bool isWebcam = data["webcam"];
    std::string droneName = data["DroneName"];
    float currentMarkerSize = data["currentMarkerSize"];
    int ArucoFront = data["ArucoIdFront"];
    int Arucoback = data["ArucoIdBehind"];

    // removed for rpi
    /*     std::string commandString = "nmcli c up " + droneName;
        const char* command = commandString.c_str();
     */
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
        std::thread movementThread([&] { webcamTest(detector); });
        movementThread.join();
    }

    else {
        // yotam code

        // removed for rpi
        /*         std::string commandString = "nmcli c up " + droneName;
                system(command);
         */

        DroneClient client(droneName, argv[2], std::stoi(argv[3]));
        client.connect_to_server();
        client.wait_for_takeoff();
        change_to_tello_wifi();

        ctello::Tello tello;

        tello.SendCommandWithResponse("streamon");

        sleep(2);

        tello.SendCommandWithResponse("takeoff");

        tello.SendCommand("rc 0 0 0 0");

        // end of yotam code

        std::string cameraString = data["cameraString"];
        int detect_class = data["detect_class"];
        aruco detector(yamlCalibrationPath, cameraString, currentMarkerSize);
        Detector object_detector(argv[1], detector.get_frame_queue());
        std::thread movementThread([&] {
            followAruco(detector, tello, ArucoFront, Arucoback, object_detector,
                        detect_class);
        });

        movementThread.join();
    }
}
