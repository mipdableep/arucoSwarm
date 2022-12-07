#include <ctello.h>
#include <unistd.h>

#include <cmath>
#include <deque>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

#include "aruco.h"
#include "constants.h"
// #include "detector.hpp"

void ScanForAruco(aruco& detector, int arucoId, bool& runDetection,
                  bool& canContinue);
void doCommand(aruco& detector, int arucoId, ctello::Tello& tello,
               std::string command, float amountOfSleepTime);
void searchArucoTargetThread(ctello::Tello& tello, aruco& detector, int ArucoTarget);
