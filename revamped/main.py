import cv2
import numpy as np
import djitellopy
from TelloObject import TelloObject
from ArucoTools import ArucoTools
from SwarmControl import SwarmControl
from time import sleep

"""
a1 = ArucoTools("Camera Calibration/calib/calib1.yaml", 1)
tello1 = TelloObject("10.3.141.101", 11111,  a1, 1, None)
a2 = ArucoTools("Camera Calibration/calib/calib2.yaml", 2)
tello2 = TelloObject("10.3.141.102", 11112,  a2, 2, None)
a3 = ArucoTools("Camera Calibration/calib/calib3.yaml", 3)
tello3 = TelloObject("10.3.141.103", 11113,  a3, 3, None)
a4 = ArucoTools("Camera Calibration/calib/calib4.yaml", 4)
tello4 = TelloObject("10.3.141.104", 11114,  a4, 4, None)
a5 = ArucoTools("Camera Calibration/calib/calib5.yaml", 5)
tello5 = TelloObject("10.3.141.105", 11115,  a5, 5, None)
"""

a2 = ArucoTools("Camera Calibration/calib/calib2.yaml", 2)
tello2 = TelloObject("10.3.141.102", 11112,  a2, 2, None)
a3 = ArucoTools("Camera Calibration/calib/calib3.yaml", 3)
tello3 = TelloObject("10.3.141.103", 11113,  a3, 3, None)
a5 = ArucoTools("Camera Calibration/calib/calib5.yaml", 5)
tello5 = TelloObject("10.3.141.105", 11115,  a5, 5, None)


# SC = SwarmControl([tello1, tello2, tello3, tello4, tello5])
SC = SwarmControl([tello2, tello3, tello5])
SC.set_dist((80, 45), (80, -45))
SC.do_for_all("getBattery()")

sleep(2)

SC.do_for_all_in_threads("startCam")

sleep(2)

SC.do_for_all_in_threads("stop")
SC.do_for_all_in_threads("takeoff")
SC.do_for_all_in_threads("stop")

if False:
    SC.do_tello_command_for_all_in_threads("rotate_clockwise", (180,))
    SC.do_tello_command_for_all_in_threads("move_forward", (75,))

sleep(2)

SC.do_tello_command_for_all_in_threads("move_up", (125,))
sleep(1)

ret = SC.all_find_target(665, 17.5)

if not ret:
    SC.do_for_all_in_threads("streamOff")
    SC.do_for_all_in_threads("kill")
    exit(0)

ret = SC.go_into_formation(tello2, tello3, tello5)

if not ret:
    SC.do_for_all_in_threads("streamOff")
    SC.do_for_all_in_threads("kill")
    exit(0)

ret = SC.go_to_target({"lr":0, "fb":150, "ud":30}, tello2, tello3, tello5)

if not ret:
    SC.do_for_all_in_threads("streamOff")
    SC.do_for_all_in_threads("kill")
    exit(0)

SC.do_for_all_in_threads("streamOff")
SC.do_for_all_in_threads("kill")

exit(0)

# tello obj for command string copy (delete)
T = djitellopy.Tello()
