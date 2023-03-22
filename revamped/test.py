import cv2
import numpy as np
from djitellopy import Tello
from TelloObject import TelloObject
from ArucoTools import ArucoTools
from SwarmControl import SwarmControl
from time import sleep

a1 = ArucoTools(30, "Camera Calibration/Calib1/Calibration.npy")
a2 = ArucoTools(30, "Camera Calibration/Calib2/Calibration.npy")
a3 = ArucoTools(30, "Camera Calibration/Calib3/Calibration.npy")

tello1 = TelloObject("10.3.141.101", 11111, a1, 1)
tello2 = TelloObject("10.3.141.102", 11112, a2, 2)
tello3 = TelloObject("10.3.141.103", 11113, a3, 3)

SC = SwarmControl([tello1, tello2, tello3])
SC.do_for_all("getBattery")

SC.do_for_all_in_threads("stop")
SC.do_for_all_in_threads("takeoff")
SC.do_for_all_in_threads("stop")
sleep(1)
SC.do_for_all("printHeight")
sleep(1)
SC.do_for_all_in_threads("setHeight", (150,))
SC.do_for_all_in_threads("stop")
sleep(4)

SC.do_for_all("printHeight")
sleep(2)

SC.exit()