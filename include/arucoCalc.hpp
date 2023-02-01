#include <cmath>
#include <iostream>

class arucoCalc {
    public:
    //calculations
        arucoCalc(int x_target ,int y_target ,int z_target);
        
        void calculate_current_wanted_Zr();
        int calculate_y_rc();
        int calculate_z_rc();
        int calculate_x_rc();
        int calculate_z_rotation_rc();

        void get_target_vals();

        /// variables
        double droneZRotate, droneXPos, droneYPos, droneZPos;
        double current_wanted_Zr;
        bool opposite_droneZr, opposite_droneZ, opposite_droneX;

    private:
    // variable declaretion:

        double RADIANS_TO_DEGREESE = (180/3.141592653589793238463);

        //target!
        int X_TARGET;
        int Y_TARGET;
        int Z_TARGET;

        double Z_ANGLE_TARGET;

        int X_LIMIT_RC = 15;
        int Y_LIMIT_RC = 15;//change
        int Z_LIMIT_RC = 15;

        void calculate_z_angle_target();
        bool opposite_position(double droneVal, double tmp);

};