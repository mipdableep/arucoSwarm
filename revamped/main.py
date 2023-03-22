import cv2
import numpy as np
from djitellopy import Tello
from TelloObject import TelloObject
from ArucoTools import ArucoTools
from SwarmControl import SwarmControl
from time import sleep
import threading

a1 = ArucoTools(30, "Camera Calibration/Calib1/Calibration.npy")
a3 = ArucoTools(30, "Camera Calibration/Calib3/Calibration.npy")
a5 = ArucoTools(30, "Camera Calibration/Calib3/Calibration.npy")

t1 = TelloObject("10.3.141.101", 11111, a1, 1)
t5 = TelloObject("10.3.141.103", 11113, a3, 2)
t3 = TelloObject("10.3.141.105", 11115, a5, 5)

SC = SwarmControl([t1, t5, t3])
SC.do_for_all("getBattery")
sleep(2)
SC.do_for_all_in_threads("startCam")
sleep(2)
SC.do_for_all_in_threads("takeoff")
sleep(1)
SC.do_tello_command_for_all_in_threads("move_up", (70,))
sleep(2)

# !first target
SC.find_aruco_all(605, 20)
SC.set_formation(SC._formation_1L, t1, t5, t3, [-100, 200, 40])
SC.move_to_target()

SC.set_formation(SC._formation_1L, t1, t5, t3, [0, 200, 40])
SC.move_to_target()


SC.find_aruco_all(685, 20)
SC.set_formation(SC._formation_1L, t3, t1, t5, [0, 200, 40])
SC.move_to_target()


SC.find_aruco_all(665, 17.5)
SC.set_formation(SC._formation_2L, t1, t5, t3, [0, 250, 40])
SC.move_to_target()


SC.find_aruco_all(615, 20)
SC.set_formation(SC._formation_1L, t5, t3, t1, [0, 400, 40])
SC.move_to_target()
SC.set_formation(SC._formation_1L, t5, t3, t1, [0, 250, 40])
SC.move_to_target()
SC.set_formation(SC._formation_1L, t5, t3, t1, [0, 150, 40])
SC.move_to_target()


SC.exit()
exit()