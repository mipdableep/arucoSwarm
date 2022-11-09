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
#define Z_TARGET 30

#define Z_ANGLE_TARGET std::atan2(X_TARGET,Y_TARGET)*RADIANS_TO_DEGREESE

#define X_LIMIT_RC 30
#define Y_LIMIT_RC 30//change
#define Z_LIMIT_RC 25

