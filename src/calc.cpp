#include "../include/calc.h"
#include "../include/constants.h"
#include <math.h>

class calc
{
private:
    /* data */
public:
    
    int calculate_y_rc(double droneYPos)
    {
        int Y_rc;
        //if current > target + tollorate
            //if bigger then rc limit
        if (droneYPos > Y_TARGET + Y_DIST_TOLORANCE){
            if ((droneYPos - Y_TARGET)/3 > Y_LIMIT_RC){
                Y_rc = -Y_LIMIT_RC;
            }
            else{
                Y_rc = -((droneYPos - Y_TARGET)/3);
            }
        }
        else
        {Y_rc = 0;}
        
        if (droneYPos < Y_TARGET - Y_DIST_TOLORANCE){
            if ((Y_TARGET - droneYPos)/3 > Y_LIMIT_RC){
                Y_rc = Y_LIMIT_RC;
            }
            else{
                Y_rc = (Y_TARGET - droneYPos)/3;
            }
        }

        Y_rc *= -1;
        return Y_rc;
    }

    int calculate_z_rc(double droneZPos)
    {
        //TODO: make it so randon jumps in z pos wont affect the commands sent to tello
        //TODO: check acuracy
        //if current > target + tollorate
            //if bigger then rc limit
        int Z_rc;

        if (droneZPos > Z_TARGET + Z_DIST_TOLORANCE){
            if ((droneZPos - Z_TARGET)/2 > Z_LIMIT_RC)
                Z_rc = -Z_LIMIT_RC;
            else
                Z_rc = -((droneZPos - Z_TARGET)/2);
        }
        else
        {Z_rc = 0;}
        
        if (droneZPos < Z_TARGET - Z_DIST_TOLORANCE){
            if ((Z_TARGET - droneZPos)/2 > Z_LIMIT_RC)
                Z_rc = Z_LIMIT_RC;
            else
                Z_rc = (Z_TARGET - droneZPos)/2;
        }

        return Z_rc;
    }

    double calculate_current_wanted_Zr(double droneXPos, double droneYPos)
    {
        return (std::atan2(droneXPos, droneYPos)*RADIANS_TO_DEGREESE);
    }

    //TODO: finish Zr calc function - aruco shuld always be in the middle
    int calculate_z_rotation_rc(double droneZRotate, double droneYPos, double droneXPos)
    {
        double current_wanted_Zr = calculate_current_wanted_Zr(droneXPos, droneYPos);
        return (droneZRotate - current_wanted_Zr);

    }

    //TODO: make x_rc calc so the the drone will go to the correct angle

    int calculate_x_rc(double droneYPos, double droneXPos)
    {
        double current_wanted_Zr = calculate_current_wanted_Zr(droneXPos, droneYPos);
        return (Z_ANGLE_TARGET-current_wanted_Zr)/2;
    }

};


