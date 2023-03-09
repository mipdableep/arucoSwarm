import cv2
import numpy as np
from TelloObject import TelloObject
from ArucoTools import ArucoTools
from SwarmControl import SwarmControl
from time import sleep

a1 = ArucoTools(100, 9.5, "Camera Calibration/Drone1/Calibration.npy")
a2 = ArucoTools(100, 9.5, "Camera Calibration/Drone2/Calibration.npy")
a3 = ArucoTools(100, 9.5, "Camera Calibration/Drone3/Calibration.npy")

tello1 = TelloObject("10.3.141.169", 11112,  26,  55, a1)
tello2 = TelloObject("10.3.141.67",  11113,   0, 100, a2)
tello3 = TelloObject("10.3.141.117", 11114, -26,  55, a3)

SC = SwarmControl([tello1, tello2, tello3])
SC.do_for_all("getBattery()")
SC.do_for_all("startCam()")

sleep(2)

SC.do_for_all_in_threads("startCam()")

sleep(2)

SC.do_for_all_in_threads("takeoff()")

sleep(1)

while cv2.waitKey(50) != ord("q"):
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    
SC.do_for_all_in_threads("kill()")