#include <cmath>

class arucoCalc {
    public:
    //calculations
        arucoCalc(int x_target ,int y_target ,int z_target);
        
        void calculate_current_wanted_Zr();
        int calculate_y_rc();
        int calculate_z_rc();
        int calculate_x_rc();
        int calculate_z_rotation_rc();

        bool check_reverse();

        void set_temp_vals();

        /// variables
        double droneZRotate, droneXPos, droneYPos, droneZPos;
        double current_wanted_Zr;
        bool opposite_droneZr, opposite_droneZ, opposite_droneX;

    private:
    // variable declaretion:

        double RADIANS_TO_DEGREESE = (180/3.141592653589793238463);

        double tmpZr, tmpZ, tmpX;

        int X_DIST_TOLORANCE =  15;//change
        int Y_DIST_TOLORANCE =  15;
        int Z_DIST_TOLORANCE =  10;

        int Z_ANGLE_TOLORANCE =  10;

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