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
tello3 = TelloObject("10.3.141.103", 11113,  a3, 3, None)


# SC = SwarmControl([tello1, tello2, tello3, tello4, tello5])
SC = SwarmControl([tello3])
sleep(1)
SC.do_for_all("getBattery()")

sleep(2)

SC.do_for_all_in_threads("startCam")

sleep(2)

tello3._arucoTool.set_target(685,17)

while cv2.waitKey(20) != ord('q'):
    s, R, T = tello3.get_location(True)
    cv2.waitKey(20)
    if s == -9:
        continue

    yaw = R["yaw"]
    lr = T["lr"]
    fb = T["fb"]
    
    wanted_yaw = math.atan2(lr, fb) * (180/math.pi)
    
    cw = int(wanted_yaw - yaw) * -1
    
    # print (R, T, "   wanted yaw: ", wanted_yaw, "  cw: ", cw)
    print ("wanted yaw: ", wanted_yaw, "  cw: ", cw)
    # print ("cw: ", cw)
    
if False:
    SC.do_for_all("takeoff")
    SC.do_movement_command_for_all_in_threads("rotate_clockwise", (180,))
    SC.do_movement_command_for_all_in_threads("move_forward", (75,))
    SC.do_for_all("kill_no_cam")

sleep(1)
SC.do_for_all_in_threads("streamOff")


# SC.do_tello_command_for_all_in_threads("reboot")


exit()
# tello5._tello.
