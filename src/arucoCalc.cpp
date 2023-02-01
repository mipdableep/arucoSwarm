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

void arucoCalc::get_target_vals(){
    std::cout<< "\nX_TARGET: " << X_TARGET << std::endl;
    std::cout<< "Y_TARGET: " << Y_TARGET << std::endl;
    std::cout<< "Z_TARGET: " << Z_TARGET << std::endl;
    std::cout<< "Z_TARGET: " << Z_ANGLE_TARGET << std::endl << std::endl;
}

int arucoCalc::calculate_y_rc() {
    // if current > target + tollorate
    // if bigger then rc limit
    int return_Y_rc;
    if (droneYPos > Y_TARGET) {
        if ((droneYPos - Y_TARGET) / 3 > Y_LIMIT_RC) {
            return_Y_rc = -Y_LIMIT_RC;
            // std::cout<<1<<std::endl;
        } else {
            return_Y_rc = -((droneYPos - Y_TARGET) / 3);
            // std::cout<<2<<std::endl;
        }
    } else {
        return_Y_rc = 0;
    }

    if (droneYPos < Y_TARGET) {
        if ((Y_TARGET - droneYPos) / 3 > Y_LIMIT_RC) {
            return_Y_rc = Y_LIMIT_RC;
            // std::cout<<3<<std::endl;
        } else {
            return_Y_rc = (Y_TARGET - droneYPos) / 3;
            // std::cout<<4<<std::endl;
        }
    }

    return_Y_rc *= -1;
    return return_Y_rc;
}

int arucoCalc::calculate_z_rc() {
    // if current > target + tollorate
    // if bigger then rc limit
    int return_Z_rc;
    if (droneZPos > Z_TARGET) {
        if ((droneZPos - Z_TARGET) / 2 > Z_LIMIT_RC)
            return_Z_rc = -Z_LIMIT_RC;
        else
            return_Z_rc = -((droneZPos - Z_TARGET) / 2);
    } else {
        return_Z_rc = 0;
    }

    if (droneZPos < Z_TARGET) {
        if ((Z_TARGET - droneZPos) / 2 > Z_LIMIT_RC)
            return_Z_rc = Z_LIMIT_RC;
        else
            return_Z_rc = (Z_TARGET - droneZPos) / 2;
    }
    return return_Z_rc;
}

int arucoCalc::calculate_x_rc() { 
    int return_X_rc;
    if (droneXPos > X_TARGET) {
        if ((droneXPos - X_TARGET) / 2 > X_LIMIT_RC)
            return_X_rc = -X_LIMIT_RC;
        else
            return_X_rc = -((droneXPos - X_TARGET) / 2);
    } else {
        return_X_rc = 0;
    }

    if (droneXPos < X_TARGET) {
        if ((X_TARGET - droneXPos) / 2 > X_LIMIT_RC)
            return_X_rc = X_LIMIT_RC;
        else
            return_X_rc = (X_TARGET - droneXPos) / 2;
    }
    return return_X_rc;
}

int arucoCalc::calculate_z_rotation_rc() {
    return (droneZRotate)/1.5;
}