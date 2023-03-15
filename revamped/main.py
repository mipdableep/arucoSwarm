import cv2
import numpy as np
from TelloObject import TelloObject
from ArucoTools import ArucoTools
from SwarmControl import SwarmControl
from time import sleep

# a1 = ArucoTools(522, 11, 30, "Camera Calibration/calib/calib1.yaml", 1)

a5 =  ArucoTools(522, 11, 30, "Camera Calibration/calib/calib5.yaml", 5)
a2 =  ArucoTools(522, 11, 30, "Camera Calibration/calib/calib2.yaml", 2)
a3 =  ArucoTools(522, 11, 30, "Camera Calibration/calib/calib3.yaml", 3)
a4 =  ArucoTools(665, 20, 20, "Camera Calibration/calib/calib4.yaml", 4)
a4b = ArucoTools(685, 20, 20, "Camera Calibration/calib/calib4.yaml", 4)

# tello1 = TelloObject("10.3.141.169", 11112,  40, 90, 10, a1, 1)

# cross distances and angles
tello5 = TelloObject("10.3.141.105", 11112,  30, 105, 0, a5, 5, 30)
tello2 = TelloObject("10.3.141.102", 11113,   0, 140, 0, a2, 2, 30)
tello3 = TelloObject("10.3.141.103", 11114, -30, 105, 0, a3, 3, 30)
tello4 = TelloObject("10.3.141.104", 11115,   0, 100, 0, a4, 4, 0 , True, 1.4)



# SC = SwarmControl([tello5, tello2, tello3, tello4])
SC = SwarmControl([tello5, tello2, tello3, tello4])
EXIT = False

SC.do_for_all("getBattery()")


sleep(2)

SC.do_for_all_in_threads("startCam")

sleep(2)

SC.do_for_all_in_threads("takeoff")

sleep(1)

for i in range(3):
    sleep(1)
    SC.do_for_all_in_threads("stop")


for i in range (1):
    
    while not tello4.target_reached:
        bias = tello4.trackLoop()
        tello5.trackLoop(bias)
        tello2.trackLoop(bias)
        tello3.trackLoop(bias)
        if cv2.waitKey(50) != ord("q"):
            EXIT = True
            break

    if EXIT:
        break
    # TODO: check if 1 second is enough
    SC.do_for_all_in_threads("stop")
    sleep(1)
    SC.do_for_all_in_threads("upup")
    tello4._arucoTool = a4b
    tello4.target_reached = False

    # TODO: test!!
    for i in range(200):
        bias = (0, 15, 0)
        tello4._tello.send_rc_control(0,20,0,0)
        tello5.trackLoop(bias)
        tello2.trackLoop(bias)
        tello3.trackLoop(bias)
        if cv2.waitKey(50) != ord("q"):
            EXIT = True
            break

    if EXIT:
        break

    while cv2.waitKey(50) != ord("q") and not tello4.target_reached:
        bias = tello4.trackLoop()
        tello5.trackLoop(bias)
        tello2.trackLoop(bias)
        tello3.trackLoop(bias)


SC.do_for_all_in_threads("streamOff")
SC.do_for_all_in_threads("kill")
