import cv2
import numpy as np
from djitellopy import Tello
from TelloObject import TelloObject
from ArucoTools import ArucoTools
from SwarmControl import SwarmControl
from time import sleep
import threading

def rotate_func (tello : TelloObject, ang, radius, arc):
    tello.rotate(ang, radius, arc, 15)

def rotate_self (tello : TelloObject, ang):
    tello._tello.rotate_clockwise (ang)

def move_up (tello : TelloObject, up):
    tello._tello.move_up (up)


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

a1 = ArucoTools(685, 17.0, 30, "Camera Calibration/Calib1/Calibration.npy")
a2 = ArucoTools(685, 17.0, 30, "Camera Calibration/Calib2/Calibration.npy")
a3 = ArucoTools(685, 17.0, 30, "Camera Calibration/Calib3/Calibration.npy")

tello1 = TelloObject("10.3.141.101", 11111, [0,0,0], a1, 1)
tello2 = TelloObject("10.3.141.102", 11112, [0,0,0], a2, 2)
tello3 = TelloObject("10.3.141.103", 11113, [0,0,0], a3, 3)

SC = SwarmControl([tello1, tello2, tello3])
SC.do_for_all("getBattery")

sleep(2)

SC.do_for_all_in_threads("startCam")

sleep(2)

SC.do_for_all_in_threads("takeoff")

sleep(1)

move_up_cm = 75

SC.do_tello_command_for_all_in_threads("move_up", (move_up_cm,))

sleep(2)

t1r, t2r, t3r = -1, -1, -1
while cv2.waitKey(50) != ord("q") and ((t1r == -1 or t2r == -1 or t3r == -1) or (t1r > 3 or t2r > 3 or t3r > 3)):
    t1r = tello1.search_aruco (665)
    t2r = tello2.search_aruco (665)
    t3r = tello3.search_aruco (665)

"""
center = np.array([0, 0, 0], dtype=np.float64)

for tello in [tello1, tello2, tello3, tello4]:
    center += np.array(tello._arucoTool.pos, dtype=np.float64)

center /= 4

def angTello(tello : TelloObject):
    return np.arctan2(center[0] - tello._arucoTool.pos[0], center[2] - tello._arucoTool.pos[2])

tellos = [tello1, tello2, tello3, tello4]
tellos = sorted(tellos, key=angTello)

center[1] = -30

ang = np.pi / 4
for tello in tellos:
    tello._position = center + np.array([42*np.sin(ang), 0, 42*np.cos(ang)])
    ang += np.pi / 2

while cv2.waitKey(50) != ord("q"):
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    tello4.trackLoop()

dist = np.sqrt(center[0] ** 2 + center[2] ** 2)
center = [0, -30, -150]

ang = np.pi / 4
for tello in tellos:
    tello._position = center + np.array([42*np.sin(ang), 0, 42*np.cos(ang)])
    ang += np.pi / 2

while cv2.waitKey(50) != ord("q"):
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    tello4.trackLoop()


SC.do_for_all_in_threads("kill")

exit(0)
"""

sleep(2)

SC.set_target_all(665, 17.5)

# !CHANGE!!!
SC.set_formation(SC._formation_1L, tello1, tello2, tello3, [0, 350, 30])
# !CHANGE!!!

next = False
while cv2.waitKey(50) != ord("q") and not next:
    r1 = tello1.trackLoop()
    r2 = tello2.trackLoop()
    r3 = tello3.trackLoop()
    next = r1 and r2 and r3

if not next:
    SC.exit()

sleep(1)

SC.set_formation(SC._formation_1L, tello1, tello2, tello3, [0, 250, 30])

next = False
while cv2.waitKey(50) != ord("q") and not next:
    r1 = tello1.trackLoop()
    r2 = tello2.trackLoop()
    r3 = tello3.trackLoop()
    next = r1 and r2 and r3

if not next:
    SC.exit()

sleep(1)

SC.set_formation(SC._formation_1L, tello1, tello2, tello3, [0, 150, 30])

next = False
while cv2.waitKey(50) != ord("q") and not next:
    r1 = tello1.trackLoop()
    r2 = tello2.trackLoop()
    r3 = tello3.trackLoop()
    next = r1 and r2 and r3

if not next:
    SC.exit()


# * NEXT TARGET
SC.set_target_all(685, 20)

t1r, t2r, t3r = -1, -1, -1
while cv2.waitKey(50) != ord("q") and ((t1r == -1 or t2r == -1 or t3r == -1) or (t1r > 3 or t2r > 3 or t3r > 3)):
    t1r = tello1.search_aruco (685)
    t2r = tello2.search_aruco (685)
    t3r = tello3.search_aruco (685)

SC.set_target_all(685, 20)

SC.set_formation(SC._formation_2L, tello2, tello3, tello1, [0, 350, 30])

next = False
while cv2.waitKey(50) != ord("q") and not next:
    r1 = tello1.trackLoop()
    r2 = tello2.trackLoop()
    r3 = tello3.trackLoop()
    next = r1 and r2 and r3

if not next:
    SC.exit()

sleep(1)

SC.set_formation(SC._formation_2L, tello2, tello3, tello1, [0, 250, 30])

next = False
while cv2.waitKey(50) != ord("q") and not next:
    r1 = tello1.trackLoop()
    r2 = tello2.trackLoop()
    r3 = tello3.trackLoop()
    next = r1 and r2 and r3

if not next:
    SC.exit()

sleep(1)

SC.set_formation(SC._formation_2L, tello2, tello3, tello1, [0, 150, 30])

next = False
while cv2.waitKey(50) != ord("q") and not next:
    r1 = tello1.trackLoop()
    r2 = tello2.trackLoop()
    r3 = tello3.trackLoop()
    next = r1 and r2 and r3


SC.exit()
exit()