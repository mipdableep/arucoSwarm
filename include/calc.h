#include "constants.h"
#include <math.h>

class calc
{
public:
    int calculate_y_rc(double droneYPos);
    int calculate_z_rc(double droneZPos);
    double calculate_current_wanted_Zr(double droneXPos, double droneYPos);
    int calculate_x_rc(double droneYPos, double droneXPos);
};
