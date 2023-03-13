import cv2
import numpy as np
from TelloObject import TelloObject
from ArucoTools import ArucoTools
from SwarmControl import SwarmControl
from time import sleep
import threading

def rotate_func (tello : TelloObject, ang, radius, arc):
    tello.rotate(ang, radius, arc, 15)

def rotate_self (tello : TelloObject, ang):
    tello._tello.rotate_clockwise (ang)

a1 = ArucoTools(522, 11, 30, "Camera Calibration/Drone1/Calibration.npy", 1)
a2 = ArucoTools(522, 11, 30, "Camera Calibration/Drone2/Calibration.npy", 2)
a3 = ArucoTools(522, 11, 30, "Camera Calibration/Drone3/Calibration.npy", 3)
a4 = ArucoTools(100, 9.5, 20, "Camera Calibration/Drone4/Calibration.npy", 4)

# tello1 = TelloObject("10.3.141.169", 11112,  40, 90, 10, a1, 1)

tello5 = TelloObject("10.3.141.105", 11112,  40, 90, 10, a1, 5)
tello2 = TelloObject("10.3.141.102",  11113,   0, 105, 10, a2, 2)
tello3 = TelloObject("10.3.141.103", 11114, -40, 90, 10, a3, 3)
tello4 = TelloObject("10.3.141.104", 11115,   0, 100, 0, a4, 4, True)


SC = SwarmControl([tello5, tello2, tello3, tello4])
SC.do_for_all("getBattery()")

if False:
    sleep(2)

    SC.do_for_all_in_threads("startCam")

    sleep(2)

    SC.do_for_all_in_threads("takeoff")

    sleep(1)

    while cv2.waitKey(50) != ord("q"):
        bias = tello4.trackLoop()
        tello1.trackLoop(bias)
        tello2.trackLoop(bias)
        tello3.trackLoop(bias)
 
    SC.do_for_all_in_threads("streamOff")
    SC.do_for_all_in_threads("kill")

else:
    SC.do_for_all_in_threads("streamOff")
