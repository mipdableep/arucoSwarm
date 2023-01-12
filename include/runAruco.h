#include <ctello.h>
#include <unistd.h>

#include <cmath>
#include <deque>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

#include "aruco.h"
#include "SerialTello.h"
#include "drone_client.hpp"
#include "arucoCalc.hpp"

#include <boost/lockfree/spsc_queue.hpp>
#include <vector>

void noLeaderLoop(aruco& detector, ctello::Tello& tello,
                  int& tmpId);

void objectOrientedNavigation(aruco& detector, ctello::Tello& tello, arucoCalc& calc);

void objectOrientedNavigation(aruco& detector, SerialTello& tello, arucoCalc& calc);

bool opposite_position(double droneVal, double tmp);

int X_rc, Y_rc, Z_rc, Zr_rc;
