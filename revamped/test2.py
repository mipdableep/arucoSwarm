from TelloObject import TelloObject
from ArucoTools import ArucoTools
from SwarmControl import SwarmControl
from time import sleep
import cv2
import math


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

a3 = ArucoTools("Camera Calibration/calib/calib3.yaml", 3)
tello3 = TelloObject("10.3.141.103", 11113,  a3, 3, None, True)

a3.set_target(665, 20)
tello3.getBattery()
sleep(3)
tello3.startCam()
sleep(2)
tello3.takeoff()

while cv2.waitKey(20) != ord('q'):
    tello3.trackLoop({"lr":0, "fb":100, "ud":0})

tello3.stop()
tello3.streamOff()
tello3.kill()


# SC.do_tello_command_for_all_in_threads("reboot")


exit()
# tello5._tello.
