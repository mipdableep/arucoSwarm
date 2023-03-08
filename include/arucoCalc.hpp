#include <cmath>
#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

class arucoCalc {
    public:
    //calculations
        arucoCalc(int x_target ,int y_target ,int z_target);
        
        int calculate_rc(double pos, int target, int rc_max, int rc_min, double devidor, int tolorance);
        int calculate_y_rc();
        int calculate_z_rc();
        int calculate_x_rc();
        int calculate_z_rotation_rc();

        void rotate_x_y_for_angle();

        void get_target_vals();
        
        void setDevidors(int dev_x, int dev_y, int dev_z);
        void setDevidors(int dev);

        void setMaxRc(int max_x, int max_y, int max_z);
        void setMaxRc(int max);

        /// variables
        double droneZRotate, droneXPos, droneYPos, droneZPos;
        double current_wanted_Zr;
        bool opposite_droneZr, opposite_droneZ, opposite_droneX;

    private:
    // variable declaretion:

        double RADIANS_TO_DEGREESE = (180/3.141592653589793238463);

        //tolorance - set in cm to each side
        int X_TOLORANCE = 5;
        int Y_TOLORANCE = 5;
        int Z_TOLORANCE = 5;

        //target!
        int X_TARGET;
        int Y_TARGET;
        int Z_TARGET;

        double Z_ANGLE_TARGET;

        // devidors - rc +- target / devidor
        double X_DEVIDOR = 1;
        double Y_DEVIDOR = 1;
        double Z_DEVIDOR = 1;

        double Z_ROTATION_DEVIDOR = 2;

        // rc limits
        int X_MAX_RC = 35;
        int Y_MAX_RC = 35;//change
        int Z_MAX_RC = 35;

        int X_MIN_RC = 0;
        int Y_MIN_RC = 0;
        int Z_MIN_RC = 0;

        void calculate_z_angle_target();
};