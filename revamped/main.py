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

def move_up (tello : TelloObject, up):
    tello._tello.move_up (up)

a1 = ArucoTools(685, 17.0, 30, "Camera Calibration/Calib1/Calibration.npy")
a2 = ArucoTools(685, 17.0, 30, "Camera Calibration/Calib2/Calibration.npy")
a3 = ArucoTools(685, 17.0, 30, "Camera Calibration/Calib3/Calibration.npy")
a4 = ArucoTools(685, 17.0, 30, "Camera Calibration/Calib4/Calibration.npy")

tello1 = TelloObject("10.3.141.101", 11112, [0,0,0], a1)
tello2 = TelloObject("10.3.141.102", 11113, [0,0,0], a2)
tello3 = TelloObject("10.3.141.103", 11114, [0,0,0], a3)
tello4 = TelloObject("10.3.141.104", 11115, [0,0,0], a4)

SC = SwarmControl([tello1, tello2, tello3, tello4])
SC.do_for_all("getBattery()")

sleep(2)

SC.do_for_all_in_threads("startCam")

sleep(2)

SC.do_for_all_in_threads("takeoff")

sleep(1)

move_up_cm = 100

thread1 = threading.Thread(target=move_up, args=(tello1, move_up_cm), name="tello1 move up")
thread2 = threading.Thread(target=move_up, args=(tello2, move_up_cm), name="tello2 move up")
thread3 = threading.Thread(target=move_up, args=(tello3, move_up_cm), name="tello3 move up")
thread4 = threading.Thread(target=move_up, args=(tello4, move_up_cm), name="tello4 move up")

thread1.start()
thread2.start()
thread3.start()
thread4.start()

thread1.join()
thread2.join()
thread3.join()
thread4.join()

sleep(2)

t1r, t2r, t3r, t4r = -1, -1, -1, -1
while cv2.waitKey(50) != ord("q") and ((t1r == -1 or t2r == -1 or t3r == -1 or t4r == -1) or (t1r > 3 or t2r > 3 or t3r > 3 or t4r > 3)):
    t1r = tello1.search_aruco (685)
    t2r = tello2.search_aruco (685)
    t3r = tello3.search_aruco (685)
    t4r = tello4.search_aruco (685)

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

sleep(2)

a1._TargetID = 665
a2._TargetID = 665
a3._TargetID = 665
tello1._position = [-30, -30, -360]
tello2._position = [ 30, -30, -360]
tello3._position = [  0, -30, -400]

while cv2.waitKey(50) != ord("q"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()

sleep(1)

a1._TargetID = 665
a2._TargetID = 665
a3._TargetID = 665
tello1._position = [-30, -30, -230]
tello2._position = [ 30, -30, -230]
tello3._position = [  0, -30, -270]

while cv2.waitKey(50) != ord("q"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()

a1._TargetID = 665
a2._TargetID = 665
a3._TargetID = 665
tello1._position = [-30, -30, -100]
tello2._position = [ 30, -30, -100]
tello3._position = [  0, -30, -140]

while cv2.waitKey(50) != ord("q"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()

a1._TargetID = 665
a2._TargetID = 665
a3._TargetID = 665
tello1._position = [-30, -40, -120]
tello2._position = [  0, -40,  -80]
tello3._position = [ 30, -40, -120]

while cv2.waitKey(50) != ord("q"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()

a1._TargetID = 665
a2._TargetID = 665
a3._TargetID = 665
tello1._position = [  0, -40, -140]
tello2._position = [-30, -40, -100]
tello3._position = [ 30, -40, -100]

while cv2.waitKey(50) != ord("q"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()

thread1 = threading.Thread(target=rotate_self, args=(tello1, 240), name="tello1 rotate")
thread2 = threading.Thread(target=rotate_self, args=(tello2, 230), name="tello2 rotate")
thread3 = threading.Thread(target=rotate_self, args=(tello3, 250), name="tello3 rotate")

thread1.start()
thread2.start()
thread3.start()

thread1.join()
thread2.join()
thread3.join()

sleep(2)

a1._TargetID = 594
a2._TargetID = 594
a3._TargetID = 594
tello1._position = [-30, -30, -360]
tello2._position = [ 30, -30, -360]
tello3._position = [  0, -30, -400]

while cv2.waitKey(50) != ord("q"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()

sleep(1)

a1._TargetID = 594
a2._TargetID = 594
a3._TargetID = 594
tello1._position = [-30, -30, -230]
tello2._position = [ 30, -30, -230]
tello3._position = [  0, -30, -270]

while cv2.waitKey(50) != ord("q"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()

a1._TargetID = 594
a2._TargetID = 594
a3._TargetID = 594
tello1._position = [-30, -30, -100]
tello2._position = [ 30, -30, -100]
tello3._position = [  0, -30, -140]

while cv2.waitKey(50) != ord("q"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()

a1._TargetID = 594
a2._TargetID = 594
a3._TargetID = 594
tello1._position = [-60, -30, -100]
tello2._position = [ 60, -30, -100]
tello3._position = [  0, -30, -180]

while cv2.waitKey(50) != ord("q"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()

'''
thread1 = threading.Thread(target=rotate_self, args=(tello1, 200), name="tello1 rotate")
thread2 = threading.Thread(target=rotate_self, args=(tello2, 180), name="tello2 rotate")
thread3 = threading.Thread(target=rotate_self, args=(tello3, 160), name="tello3 rotate")

thread1.start()
thread2.start()
thread3.start()

thread1.join()
thread2.join()
thread3.join()

a1._TargetID = 546
a2._TargetID = 546
a3._TargetID = 546

tello1._hight = 0
tello2._hight = 0
tello3._hight = 0

while cv2.waitKey(50) != ord("q"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
'''

SC.do_for_all_in_threads("kill")