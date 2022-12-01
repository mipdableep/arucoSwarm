#include <ctello.h>
#include <unistd.h>

#include <cmath>
#include <deque>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

#include "../include/drone.h"
#include "aruco.h"
#include "constants.h"

void ScanForAruco(aruco& detector, int arucoId, bool& runDetection, bool& canContinue);
void doCommand (aruco& detector, int arucoId, ctello::Tello& tello, std::string command, float amountOfSleepTime);

