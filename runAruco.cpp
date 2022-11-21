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

				// droneZRotate = detector.yaw;
				// droneXPos = detector.rightLeft;
				// droneYPos = detector.forward;	
				// droneZPos = detector.upDown;

				// //run rc calculations
				// calculate_x_rc();
				// calculate_y_rc();
				// calculate_z_rc();
				// calculate_z_rotation_rc();


				// command = "rc ";

				// command += std::to_string((int)(X_rc));
				// command+=" ";
				
				// command += std::to_string((int)(Y_rc));
				// command+=" ";
				
				// command += std::to_string((int)(Z_rc));
				// command+=" ";
				
				// command += std::to_string((int)(Zr_rc));

				// // tello.SendCommand(command);	
				// // std::cout<<"OON command:  "<<command<<std::endl;

				// std::cout<<"Z pos:  "<<droneZPos<<std::endl;
				// std::cout<<"Z rc:   "<<Z_rc<<std::endl<<std::endl;


			}
			else{
				std::cout<<"in else"<<std::endl;
			}

			commandFlag = false;

		}
		sleep(1);
	}
}

void objectOrientedNavigation(drone& drone, aruco& detector, ctello::Tello& tello, calc calculator)
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

				double tmpZr = droneZRotate, tmpZ = droneZPos, tmpX = droneXPos;

				bool made_switch_last_time;

				droneZRotate = detector.yaw;
				droneXPos = detector.rightLeft;
				droneYPos = detector.forward;
				droneZPos = detector.upDown;

				bool opposite_droneZr, opposite_droneZ, opposite_droneX;

				opposite_droneZr = opposite_position(droneZRotate, tmpZr);
				opposite_droneZ = opposite_position(droneZPos, tmpZ);
				opposite_droneX = opposite_position(droneXPos, tmpX);

				if (opposite_droneZr && opposite_droneZ && opposite_droneX)
				{
					droneXPos *= -1;
					droneZPos *= -1;
					droneZRotate *= -1;
					std::cout<<"make switch\n"<<std::endl;
				}

				landCaseCheck(rc_y_below_land_limit, wentDownCounter, tello);

				//run rc calculations
				X_rc = calculator.calculate_x_rc(droneYPos, droneXPos);
				Y_rc = calculator.calculate_y_rc(droneYPos);
				Z_rc = calculator.calculate_z_rc(droneZPos);
				Zr_rc = calculator.calculate_z_rotation_rc(droneZRotate, droneYPos, droneXPos);


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


bool opposite_position(double droneVal, double tmp)
{
	return (tmp -(0.4*tmp) < -droneVal && -droneVal < tmp + (0.4*tmp));
}


bool opposite_angle(double droneVal, double tmp)
{
	return (tmp -(0.7*tmp) < -droneVal && -droneVal < tmp + (0.7*tmp));
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
		calc calculator;
		tello.SendCommand("rc 0 0 0 0");
		std::string cameraString = data["cameraString"];
		aruco detector(yamlCalibrationPath,cameraString,currentMarkerSize);
		std::thread movementThread([&] { objectOrientedNavigation(d1, detector,tello, calculator); } );   
		// std::thread movementThread([&] { webcamTest(detector); } );        

		movementThread.join(); 
	}

}
	