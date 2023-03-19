import cv2
import numpy as np
from TelloObject import TelloObject
from ArucoTools import ArucoTools
from SwarmControl import SwarmControl
from time import sleep

"""
a1 = ArucoTools("Camera Calibration/calib/calib1.yaml", 1)
a2 = ArucoTools("Camera Calibration/calib/calib2.yaml", 2)
a3 = ArucoTools("Camera Calibration/calib/calib3.yaml", 3)
a4 = ArucoTools("Camera Calibration/calib/calib4.yaml", 4)
a5 = ArucoTools("Camera Calibration/calib/calib5.yaml", 5)

tello1 = TelloObject("10.3.141.101", 11111,  a1, 1, None)
tello2 = TelloObject("10.3.141.102", 11112,  a2, 2, None)
tello3 = TelloObject("10.3.141.103", 11113,  a3, 3, None)
tello4 = TelloObject("10.3.141.104", 11114,  a4, 4, None)
tello5 = TelloObject("10.3.141.105", 11115,  a5, 5, None)
"""

a5 = ArucoTools("Camera Calibration/calib/calib5.yaml", 5)
tello5 = TelloObject("10.3.141.105", 11115,  a5, 5, None)



# SC = SwarmControl([tello1, tello2, tello3, tello4, tello5])
SC = SwarmControl([tello5])
EXIT = False

SC.do_for_all("getBattery()")

sleep(2)

SC.do_for_all_in_threads("takeoff")
SC.do_for_all_in_threads("stop")
SC.do_for_all_in_threads("startCam")

sleep(1)

SC.do_movement_command_for_all_in_threads("move_up", (75,))
sleep(1)

SC.all_find_target(665, 17.5)

SC.do_movement_command_for_all_in_threads("move_forward", (150,))

SC.do_for_all_in_threads("streamOff")
SC.do_for_all_in_threads("kill")

exit(0)

# tello obj for command string copy (delete)
tello5._tello.rotate_clockwise()