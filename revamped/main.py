import cv2
import numpy as np
from TelloObject import TelloObject
from ArucoTools import ArucoTools
from SwarmControl import SwarmControl
from time import sleep

a1 = ArucoTools(522, 7.0, "Camera Calibration/Drone1/Calibration.npy")
a2 = ArucoTools(522, 7.0, "Camera Calibration/Drone2/Calibration.npy")
a3 = ArucoTools(522, 7.0, "Camera Calibration/Drone3/Calibration.npy")
a4 = ArucoTools(100, 9.5, "Camera Calibration/Drone3/Calibration.npy")

tello1 = TelloObject("10.3.141.169", 11112,  45,  71, a1)
tello2 = TelloObject("10.3.141.67",  11113,   0, 100, a2)
tello3 = TelloObject("10.3.141.117", 11114, -45,  71, a3)
tello4 = TelloObject("10.3.141.211", 11115,   0, 150, a4)

SC = SwarmControl([tello1, tello2, tello3, tello4])
SC.do_for_all("getBattery()")
#SC.do_for_all("startCam()")

sleep(2)

# Ask Ido : why two times startCam ?
SC.do_for_all_in_threads("startCam")
#
sleep(2)

SC.do_for_all_in_threads("takeoff")

sleep(1)

while cv2.waitKey(50) != ord("q"):
    bias = tello4.trackLoop()
    tello1.trackLoop(bias)
    tello2.trackLoop(bias)
    tello3.trackLoop(bias)
    
SC.do_for_all_in_threads("kill")