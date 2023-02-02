#include "../include/arucoCalc.hpp"

/// @brief init the calc with target parameters
/// @param x_target 
/// @param y_target 
/// @param z_target 
arucoCalc::arucoCalc(int x_target ,int y_target ,int z_target){
    X_TARGET = x_target;
    Y_TARGET = y_target;
    Z_TARGET = z_target;
    calculate_z_angle_target();
}

void arucoCalc::calculate_z_angle_target(){
    Z_ANGLE_TARGET = std::atan2(X_TARGET,Y_TARGET)*RADIANS_TO_DEGREESE;
}

void arucoCalc::get_target_vals(){ std::cout<<std::endl;
    std::cout<< "X_TARGET: " << X_TARGET << std::endl;
    std::cout<< "Y_TARGET: " << Y_TARGET << std::endl;
    std::cout<< "Z_TARGET: " << Z_TARGET << std::endl;
    std::cout<< "Z_TARGET: " << Z_ANGLE_TARGET << std::endl << std::endl;
}

int arucoCalc::calculate_rc(double pos, int target, int rc_max, int rc_min, double devidor, int tolorance){
    int rc;
    if (pos > target + tolorance)
        rc = -((pos - target) / devidor);
    
    else if (pos < target - tolorance)
        rc = (target - pos) / devidor;
    
    else
        return 0;
    
    if (rc > 0){
        if (rc < rc_min)
            rc = rc_min;
        
        if (rc > rc_max)
            rc = rc_max;

    } else { //if rc < 0
        if (rc < rc_max)
            rc = -rc_max;
        
        if (rc > rc_min)
            rc = -rc_min;
    }
}

int arucoCalc::calculate_y_rc() {
    return -(calculate_rc(droneYPos, Y_TARGET, Y_MAX_RC, Y_MIN_RC, Y_DEVIDOR, Y_TOLORANCE));
}

int arucoCalc::calculate_z_rc() {
    return calculate_rc(droneZPos, Z_TARGET, Z_MAX_RC, Z_MIN_RC, Z_DEVIDOR, Z_TOLORANCE);
}

int arucoCalc::calculate_x_rc() { 
    return -(calculate_rc(droneXPos, X_TARGET, X_MAX_RC, X_MIN_RC, X_DEVIDOR, X_TOLORANCE));
}