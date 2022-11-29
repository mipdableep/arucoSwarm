#pragma once
#include <iostream>
//ID = 

//right to left: 1 to 3
//1: 35,  90,  15
//2: 0,   110, 25
//3: -35, 90,  15

//newsystem constants
#define RADIANS_TO_DEGREESE (180.0/3.141592653589793238463)

#define X_DIST_TOLORANCE 15//change
#define Y_DIST_TOLORANCE 15
#define Z_DIST_TOLORANCE 10

#define Z_ANGLE_TOLORANCE 10

//target!
#define X_TARGET -35
#define Y_TARGET 90
#define Z_TARGET 15

#define Z_ANGLE_TARGET std::atan2(X_TARGET,Y_TARGET)*RADIANS_TO_DEGREESE

#define X_LIMIT_RC 15
#define Y_LIMIT_RC 15//change
#define Z_LIMIT_RC 15

