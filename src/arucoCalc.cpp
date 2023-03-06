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

void arucoCalc::chagneDevidor(int dev_x, int dev_y, int dev_z){
    X_DEVIDOR = dev_x;
    Y_DEVIDOR = dev_y;
    Z_DEVIDOR = dev_z;
}

void arucoCalc::chagneDevidor(int dev){
    X_DEVIDOR = dev;
    Y_DEVIDOR = dev;
    Z_DEVIDOR = dev;
}

void arucoCalc::changeMax(int max_x, int max_y, int max_z){
    X_MAX_RC = max_x;
    Y_MAX_RC = max_y;
    Z_MAX_RC = max_z;
}

void arucoCalc::changeMax(int max){
    X_MAX_RC = max;
    Y_MAX_RC = max;
    Z_MAX_RC = max;
}

// int arucoCalc::calculate_rc(double pos, int target, int rc_max, int rc_min, double devidor, int tolorance){
//     int rc;
//     if (pos > target + tolorance)
//         rc = -((pos - target) / devidor);

//     else if (pos < target - tolorance)
//         rc = (target - pos) / devidor;

//     else
//         return 0;

//     if (rc > 0){
//         if (rc < rc_min)
//             rc = rc_min;

//         if (rc > rc_max)
//             rc = rc_max;

//     } else { //if rc < 0
//         if (rc < rc_max)
//             rc = -rc_max;

//         if (rc > rc_min)
//             rc = -rc_min;
//     }
//     return rc;
// }

int arucoCalc::calculate_rc(double pos, int target, int rc_max, int rc_min, double devidor, int tolorance) {
    int rc = 0;
    double deviation = pos - target;

    if (std::abs(deviation) > tolorance) {
        rc = deviation / devidor;
        rc = std::clamp(rc, -rc_max, rc_max);
    }

    return rc;
}

int arucoCalc::calculate_y_rc() {
    return (calculate_rc(droneYPos, Y_TARGET, Y_MAX_RC, Y_MIN_RC, Y_DEVIDOR, Y_TOLORANCE));
}

int arucoCalc::calculate_z_rc() {
    return -calculate_rc(droneZPos, Z_TARGET, Z_MAX_RC, Z_MIN_RC, Z_DEVIDOR, Z_TOLORANCE);
}

int arucoCalc::calculate_x_rc() {
    return -calculate_rc(droneXPos, X_TARGET, X_MAX_RC, X_MIN_RC, X_DEVIDOR, X_TOLORANCE);
}

int arucoCalc::calculate_z_rotation_rc() {
    return (droneZRotate)/Z_ROTATION_DEVIDOR;
}

// int arucoCalc::calculate_y_rc() {
//     // if current > target + tollorate
//     // if bigger then rc limit
//     int return_Y_rc;
//     if (droneYPos > Y_TARGET + Y_TOLORANCE) {
//         if ((droneYPos - Y_TARGET) / 3 > Y_MAX_RC) {
//             return_Y_rc = -Y_MAX_RC;
//             // std::cout<<1<<std::endl;
//         } else {
//             return_Y_rc = -((droneYPos - Y_TARGET) / 3);
//             // std::cout<<2<<std::endl;
//         }
//     } else {
//         return_Y_rc = 0;
//     }
//
//     if (droneYPos < Y_TARGET - Y_TOLORANCE) {
//         if ((Y_TARGET - droneYPos) / 3 > Y_MAX_RC) {
//             return_Y_rc = Y_MAX_RC;
//             // std::cout<<3<<std::endl;
//         } else {
//             return_Y_rc = (Y_TARGET - droneYPos) / 3;
//             // std::cout<<4<<std::endl;
//         }
//     }

//     return_Y_rc *= -1;
//     return return_Y_rc;
// }

// int arucoCalc::calculate_z_rc() {
//     // if current > target + tollorate
//     // if bigger then rc limit
//     int return_Z_rc;
//     if (droneZPos > Z_TARGET + Z_TOLORANCE) {
//         if ((droneZPos - Z_TARGET) / 2 > Z_MAX_RC)
//             return_Z_rc = -Z_MAX_RC;
//         else
//             return_Z_rc = -((droneZPos - Z_TARGET) / 2);
//     } else {
//         return_Z_rc = 0;
//     }

//     if (droneZPos < Z_TARGET - Z_TOLORANCE) {
//         if ((Z_TARGET - droneZPos) / 2 > Z_MAX_RC)
//             return_Z_rc = Z_MAX_RC;
//         else
//             return_Z_rc = (Z_TARGET - droneZPos) / 2;
//     }
//     return return_Z_rc;
// }

// int arucoCalc::calculate_x_rc() {
//     // if current > target + tollorate
//     // if bigger then rc limit
//     int return_X_rc;
//     if (droneXPos > X_TARGET + X_TOLORANCE) {
//         if ((droneXPos - X_TARGET) / 2 > X_MAX_RC)
//             return_X_rc = -X_MAX_RC;
//         else
//             return_X_rc = -((droneXPos - X_TARGET) / 2);
//     } else {
//         return_X_rc = 0;
//     }

//     if (droneXPos < X_TARGET - X_TOLORANCE) {
//         if ((X_TARGET - droneXPos) / 2 > X_MAX_RC)
//             return_X_rc = X_MAX_RC;
//         else
//             return_X_rc = (X_TARGET - droneXPos) / 2;
//     }
//     return return_X_rc;
// }

// int arucoCalc::calculate_z_rotation_rc() {
//     return (droneZRotate)/1.5;
// }