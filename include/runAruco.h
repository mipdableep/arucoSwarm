#include <ctello.h>
#include <unistd.h>

#include <cmath>
#include <deque>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

#include "aruco.h"
#include "constants.h"
#include "drone_client.hpp"


void noLeaderLoop(aruco& detector, ctello::Tello& tello,
                  int& tmpId);
void noLeaderLoop_v2(aruco& detector, ctello::Tello& tello,
                     int& tmpId);

void landCaseCheck(int& rc_y_below_land_limit, int& wentDownCounter, ctello::Tello& tello);

void objectOrientedNavigation(aruco& detector, ctello::Tello& tello);

void detectorLeaderLoop(ctello::Tello& tello);

void calculate_y_rc();
void calculate_z_rc();
void calculate_current_wanted_Zr();
void calculate_z_rotation_rc();
void calculate_x_rc();

bool opposite_position(double droneVal, double tmp);
