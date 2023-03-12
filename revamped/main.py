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

a1 = ArucoTools(549, 15.0, 30, "Camera Calibration/Calib1/Calibration.npy")
a2 = ArucoTools(549, 15.0, 30, "Camera Calibration/Calib2/Calibration.npy")
a3 = ArucoTools(549, 15.0, 30, "Camera Calibration/Calib3/Calibration.npy")
a4 = ArucoTools(100,  9.5, 20, "Camera Calibration/Calib4/Calibration.npy")

tello1 = TelloObject("10.3.141.169", 11112,  26, 111, 10, a1)
tello2 = TelloObject("10.3.141.67",  11113,   0, 150, 10, a2)
tello3 = TelloObject("10.3.141.117", 11114, -26, 111, 10, a3)
# tello4 = TelloObject("10.3.141.211", 11115,   0, 100, a4)

SC = SwarmControl([tello1, tello2, tello3])
SC.do_for_all("getBattery()")

sleep(2)

SC.do_for_all_in_threads("startCam")

sleep(2)

SC.do_for_all_in_threads("takeoff")

sleep(1)

while cv2.waitKey(50) != ord("q"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()

thread1 = threading.Thread(target=rotate_func, args=(tello1, 64, 52, 180), name="tello1 rotate")
thread2 = threading.Thread(target=rotate_func, args=(tello2,  0, 52, 180), name="tello2 rotate")
thread3 = threading.Thread(target=rotate_func, args=(tello3,-64, 52, 180), name="tello3 rotate")

thread1.start()
thread2.start()
thread3.start()

thread1.join()
thread2.join()
thread3.join()

thread1 = threading.Thread(target=rotate_self, args=(tello1, 180), name="tello1 rotate")
thread2 = threading.Thread(target=rotate_self, args=(tello2, 180), name="tello2 rotate")
thread3 = threading.Thread(target=rotate_self, args=(tello3, 180), name="tello3 rotate")

thread1.start()
thread2.start()
thread3.start()

thread1.join()
thread2.join()
thread3.join()

while cv2.waitKey(50) != ord("q"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()

SC.do_for_all_in_threads("kill")