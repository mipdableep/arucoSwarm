from TelloObject import TelloObject
from ArucoTools import ArucoTools
from SwarmControl import SwarmControl
from time import sleep

# a1 = ArucoTools(522, 11, 30, "Camera Calibration/calib/calib1.yaml", 1)

a5 =  ArucoTools(522, 11, 30, "Camera Calibration/calib/calib5.yaml", 5)
a2 =  ArucoTools(522, 11, 30, "Camera Calibration/calib/calib2.yaml", 2)
a3 =  ArucoTools(522, 11, 30, "Camera Calibration/calib/calib3.yaml", 3)
a4 =  ArucoTools(665, 20, 20, "Camera Calibration/calib/calib4.yaml", 4)
# a4b = ArucoTools(685, 20, 20, "Camera Calibration/calib/calib4.yaml", 4)

# tello1 = TelloObject("10.3.141.169", 11112,  40, 90, 10, a1, 1)

# cross distances and angles
tello5 = TelloObject("10.3.141.105", 11112,  30, 105, 0, a5, 5, 30)
tello2 = TelloObject("10.3.141.102", 11113,   0, 140, 0, a2, 2, 30)
tello3 = TelloObject("10.3.141.103", 11114, -30, 105, 0, a3, 3, 30)
tello4 = TelloObject("10.3.141.104", 11115,   0, 100, 0, a4, 4, 0 , True, 1.4)


# SC = SwarmControl([tello5, tello2, tello3, tello4])
SC = SwarmControl([tello5, tello2, tello3, tello4])
sleep(1)
SC.do_for_all("getBattery()")
sleep(1)
SC.do_for_all_in_threads("streamOff")
