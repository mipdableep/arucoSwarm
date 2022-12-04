#include <ctello.h>
#include <unistd.h>

#include <cmath>
#include <deque>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

#include "aruco.h"
#include "constants.h"

void ScanForAruco(aruco& detector, int arucoId, bool& runDetection, bool& canContinue);
void doContinuanceCommand (aruco& detector, int arucoId, ctello::Tello& tello, std::string command, float amountOfSleepTime);
void scanForward (aruco& detector, int arucoId, ctello::Tello& tello);