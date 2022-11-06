#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>
#include <unistd.h>
#include <deque>
#include "aruco.h"
#include "../include/drone.h"
#include <cmath>
#include <ctello.h>

void noLeaderLoop(drone& drone, aruco& detector, ctello::Tello& tello, int& tmpId, int& sleepAmount);

void calculate_y_rc();
void calculate_z_rc();
void calculate_current_wanted_Zr();
void calculate_z_rotation_rc();
void calculate_x_rc();


void noLeaderLoop(drone& drone, aruco& detector, ctello::Tello& tello, int& tmpId, int& sleepAmount)
{


	if(detector.ID==-1 && tmpId!=-1)
	{
		//wait to see if problem solevs itself
		tello.SendCommand("rc 0 0 0 0");
		printf("sleeping: %d seconds", sleepAmount);
		sleep(sleepAmount);

		int i=0;
		while(detector.ID==-1 && i<50)
		{
			tello.SendCommand("rc 0 0 0 0");
			std::cout << "Searching for leader" << std::endl;
			tello.SendCommand("rc 0 0 0 25");
				i++;
				if(detector.ID!=tmpId && detector.ID!=-1)
						detector.init=true;
			sleep(2); 
		}
		
		if(detector.ID==-1)
			tello.SendCommandWithResponse("land");    			
	}
}


void calculate_y_rc()
{
	//if current > target + tollorate
		//if bigger then rc limit
	if (droneYPos > Y_TARGET + Y_DIST_TOLORANCE){
		if ((droneYPos - Y_TARGET)/3 > Y_LIMIT_RC)
			Y_rc = -Y_LIMIT_RC;
		else
			Y_rc = -((droneYPos - Y_TARGET)/3);
	}
	else
	{Y_rc = 0;}
	
	if (droneYPos < Y_TARGET - Y_DIST_TOLORANCE){
		if ((Y_TARGET - droneYPos)/3 > Y_LIMIT_RC)
			Y_rc = Y_LIMIT_RC;
		else
			Y_rc = (Y_TARGET - droneYPos)/3;
	}

	Y_rc *= -1;
}

void calculate_z_rc()
{
	//TODO: make it so randon jumps in z pos wont affect the commands sent to tello
	//TODO: check acuracy
	//TODO: change the output amount of the rc
	//if current > target + tollorate
		//if bigger then rc limit
	if (droneZPos > Z_TARGET + Z_DIST_TOLORANCE){
		if ((droneZPos - Z_TARGET)/3 > Z_LIMIT_RC)
			Z_rc = -Z_LIMIT_RC;
		else
			Z_rc = -((droneZPos - Z_TARGET)/3);
	}
	else
	{Z_rc = 0;}
	
	if (droneZPos < Z_TARGET - Z_DIST_TOLORANCE){
		if ((Z_TARGET - droneZPos)/3 > Z_LIMIT_RC)
			Z_rc = Z_LIMIT_RC;
		else
			Z_rc = (Z_TARGET - droneZPos)/3;
	}

	Z_rc *= -1;
}


void calculate_current_wanted_Zr()
{
	current_wanted_Zr = std::atan2(droneXPos, droneYPos)*RADIANS_TO_DEGREESE;
}

//TODO: finish Zr calc function - aruco shuld always be in the middle
void calculate_z_rotation_rc()
{
	calculate_current_wanted_Zr();
	Zr_rc = droneZRotate - current_wanted_Zr;

}

//TODO: make x_rc calc so the the drone will go to the correct angle

void calculate_x_rc()
{
	X_rc = (Z_ANGLE_TARGET-current_wanted_Zr)/2;
}