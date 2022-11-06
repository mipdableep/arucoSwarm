#pragma once
#include <iostream>

//newsystem constants
#define RADIANS_TO_DEGREESE (180.0/3.141592653589793238463)

#define X_DIST_TOLORANCE 15//change
#define Y_DIST_TOLORANCE 15
#define Z_DIST_TOLORANCE 10

#define Z_ANGLE_TOLORANCE 10

//target!
#define X_TARGET 0
#define Y_TARGET 100
#define Z_TARGET 0

#define Z_ANGLE_TARGET std::atan2(X_TARGET,Y_TARGET)*RADIANS_TO_DEGREESE

#define X_LIMIT_RC 20
#define Y_LIMIT_RC 20//change
#define Z_LIMIT_RC 20



//OLD SYSTEM CONSTANTS
//1 = most right, 2 = middle, 3 = most left
#define DRONE_ID 2

// const std::string noMovement = "0 ";


//checks id's == 1
#if DRONE_ID == 1
#define RIGHT_LEFT -40
#endif

#if DRONE_ID == 2
#define RIGHT_LEFT 0
#endif

#if DRONE_ID == 3
#define RIGHT_LEFT 40
#endif

//define distance from aruco in x axis (back/forward)
#define FORWARD 160
//upper limit of how fast the drone can move on x axis (in rc command)
#define LIM_FORWARD 20
//upper limit of how fast the drone can move on y axis (in rc command)
#define LIM_RIGHT_LEFT 15

 
#define LIM_HEIGHT 15
#define LIM_ANGLE 20
//#define LIM_ANGLE_CIRCLE 25
#define LIM_MOVEMENT 30
#define LIM_MOVEMENT_HEIGHT 30
#define LIM_MOVEMENT_ANGLE 25
