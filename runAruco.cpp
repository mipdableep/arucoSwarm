//
// Created by tzuk on 6/6/22.
//

#include "include/runAruco.h"
#include "include/constants.h"


//declare vars
double droneZRotate, droneXPos, droneYPos, droneZPos;
double current_wanted_Zr;
int X_rc, Y_rc, Z_rc, Zr_rc;
int noLeaderCycle = 0;
std::string command;


void webcamTest(aruco& detector)
{
	std::cout<<"started OON\n"<<std::endl;

	int tmpId=-1;
	bool commandFlag;
	//check land vriables
	int wentDownCounter = 0;
	int sleepAmount = 2;
	while (true){
		std::cout<<wentDownCounter<<std::endl;
		wentDownCounter++;
		while(commandFlag && detector.ID!=-1);
		
			if(detector.ID!=-1)
			{
				tmpId=detector.ID;
			}
		
		if (detector.ID==-1 && tmpId!=-1){
			// noLeaderLoop(drone, detector, tello, tmpId, sleepAmount);
			printf("would do noLeader\n");
		}

		else{
			if(!detector.init || detector.ID!=-1)
			{

				droneZRotate = detector.yaw;
				droneXPos = detector.rightLeft;
				droneYPos = detector.forward;	
				droneZPos = detector.upDown;

				//run rc calculations
				calculate_x_rc();
				calculate_y_rc();
				calculate_z_rc();
				calculate_z_rotation_rc();


				command = "rc ";

				command += std::to_string((int)(X_rc));
				command+=" ";
				
				command += std::to_string((int)(Y_rc));
				command+=" ";
				
				command += std::to_string((int)(Z_rc));
				command+=" ";
				
				command += std::to_string((int)(Zr_rc));

				// tello.SendCommand(command);	
				// std::cout<<"OON command:  "<<command<<std::endl;

				std::cout<<"Z pos:  "<<droneZPos<<std::endl;
				std::cout<<"Z rc:   "<<Z_rc<<std::endl<<std::endl;


			}
			else{
				std::cout<<"in else"<<std::endl;
			}

			commandFlag = false;

		}
		sleep(1);
	}
}

void objectOrientedNavigation(drone& drone, aruco& detector, ctello::Tello& tello)
{
	std::cout<<"started OON\n"<<std::endl;

	int tmpId=-1;
	bool commandFlag;
	//check land vriables
	int rc_y_below_land_limit;
	int wentDownCounter = 0;


	while (true){
		while(commandFlag && detector.ID!=-1);
		
			if(detector.ID!=-1)
			{
				tmpId=detector.ID;
			}
		
		if (detector.ID==-1 && tmpId!=-1){
			// noLeaderLoop_v2(drone, detector, tello, tmpId);
			noLeaderLoop(drone, detector, tello, tmpId);
		}

		else{
			if(!detector.init || detector.ID!=-1)
			{

				droneZRotate = detector.yaw;
				droneXPos = detector.rightLeft;
				droneYPos = detector.forward;	
				droneZPos = detector.upDown;

				//run rc calculations
				calculate_x_rc();
				calculate_y_rc();
				calculate_z_rc();
				calculate_z_rotation_rc();

				landCaseCheck(rc_y_below_land_limit, wentDownCounter, tello);

				std::string command = "rc ";

				command += std::to_string((int)(X_rc));
				command+=" ";
				
				command += std::to_string((int)(Y_rc));
				command+=" ";
				
				command += std::to_string((int)(Z_rc));
				command+=" ";
				
				command += std::to_string((int)(Zr_rc));

				tello.SendCommand(command);	
				std::cout<<"OON command:  "<<command<<std::endl;

			}
			else{
				tello.SendCommand("rc 0 0 0 0");
			}


			sleep(1);
			commandFlag = false;

		}
	}
}


void noLeaderLoop(drone& drone, aruco& detector, ctello::Tello& tello, int& tmpId)
{
	std::cout<<"in no leader loop"<<std::endl;
	int sleepAmount = 2;
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

//fuction that checks if the drone stayed at the same level, tells it to go down, and if the z pos relative to the
//navigation object didnt go down, tells it to land
void landCaseCheck(int& rc_y_below_land_limit, int& wentDownCounter, ctello::Tello& tello)
{
	if (Z_rc < -5){
		rc_y_below_land_limit ++;
	}
			
	else {
		if(rc_y_below_land_limit > 3){
			rc_y_below_land_limit -= 3;
			std::cout<<"counter went down\n";
		}
	}

	if (rc_y_below_land_limit > 16){
		int first_z_val = droneZPos;
		tello.SendCommand("rc 0 0 -40 0");
		sleep(4);
		if (droneZPos < first_z_val + 5){
			std::cout<<"landing"<<std::endl;
			tello.SendCommandWithResponse("land");
			exit(0);
		}
		else
		{
			rc_y_below_land_limit -= 6;
		}
	}
}

void calculate_y_rc()
{
	//if current > target + tollorate
		//if bigger then rc limit
	if (droneYPos > Y_TARGET + Y_DIST_TOLORANCE){
		if ((droneYPos - Y_TARGET)/3 > Y_LIMIT_RC){
			Y_rc = -Y_LIMIT_RC;
			// std::cout<<1<<std::endl;
		}
		else{
			Y_rc = -((droneYPos - Y_TARGET)/3);
			// std::cout<<2<<std::endl;
		}
	}
	else
	{Y_rc = 0;}
	
	if (droneYPos < Y_TARGET - Y_DIST_TOLORANCE){
		if ((Y_TARGET - droneYPos)/3 > Y_LIMIT_RC){
			Y_rc = Y_LIMIT_RC;
			// std::cout<<3<<std::endl;
		}
		else{
			Y_rc = (Y_TARGET - droneYPos)/3;
			// std::cout<<4<<std::endl;
		}
	}

	Y_rc *= -1;
}

void calculate_z_rc()
{
	//TODO: make it so randon jumps in z pos wont affect the commands sent to tello
	//TODO: check acuracy
	//if current > target + tollorate
		//if bigger then rc limit
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
}

void calculate_current_wanted_Zr()
{
	current_wanted_Zr = std::atan2(droneXPos, droneYPos)*RADIANS_TO_DEGREESE;
}

//TODO: finish Zr calc function - aruco shuld always be in the middle
void calculate_z_rotation_rc()
{
	calculate_current_wanted_Zr();
	Zr_rc = (droneZRotate - current_wanted_Zr);

}

//TODO: make x_rc calc so the the drone will go to the correct angle

void calculate_x_rc()
{
	X_rc = (Z_ANGLE_TARGET-current_wanted_Zr)/2;
}




void runAruco(aruco &detector, drone &d1, ctello::Tello& tello){
    while(true){
   
        if(detector.ID!=-1){        
        }
    }
}



int main(){
	
	std::ifstream programData("../config.json");
	
	nlohmann::json data;
	programData >> data;
	programData.close();
	bool isWebcam = data["webcam"];
	std::string droneName = data["DroneName"];
	float currentMarkerSize = data["currentMarkerSize"];
	std::string commandString = "nmcli c up " + droneName;
	const char *command = commandString.c_str();
	
	//checking the img input device for correct calibration
	std::string yamlCalibrationPath;
	
	if(isWebcam){
		yamlCalibrationPath = data["webcamYamlCalibrationPath"];
	}
	else{
		yamlCalibrationPath = data["yamlCalibrationPath"];
	}


	if (isWebcam){
		int cameraPort = data["cameraPort"];
		aruco detector(yamlCalibrationPath,cameraPort,currentMarkerSize);

		std::thread movementThread([&] { webcamTest(detector); } );        
		movementThread.join(); 
	}

	else
	{
		drone d1;
		std::string commandString = "nmcli c up " + droneName;
		system(command);
		ctello::Tello tello;
		tello.SendCommandWithResponse("streamon");

		sleep(2);
		tello.SendCommandWithResponse("takeoff");
		tello.SendCommand("rc 0 0 0 0");
		std::string cameraString = data["cameraString"];
		aruco detector(yamlCalibrationPath,cameraString,currentMarkerSize);
		std::thread movementThread([&] { objectOrientedNavigation(d1, detector,tello); } );   
		// std::thread movementThread([&] { webcamTest(detector); } );        

		movementThread.join(); 
	}

}
	