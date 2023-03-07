#include <ctello.h>
#include <unistd.h>

#include <cmath>
#include <deque>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

#include "../include/arucoUtils.h"
// #include "SerialTello.h"
#include "drone_client.hpp"
#include "arucoCalc.hpp"

#include <boost/lockfree/spsc_queue.hpp>
#include <vector>

void objectOrientedNavigation(aruco_utils& detector, ctello::Tello& tello, arucoCalc& calc);

void timer_limiter(aruco_utils& detector, ctello::Tello& tello, bool& run_OON, int seconds_time_amount);

void exit_all(aruco_utils& detector, bool& run_OON, ctello::Tello& tello);
void exit_all(aruco_utils& detector, bool& run_OON);

int X_rc, Y_rc, Z_rc, Zr_rc;

bool run_OON = true;