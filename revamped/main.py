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

a1 = ArucoTools(685, 17.25, 30, "Camera Calibration/Calib1/Calibration.npy")
a2 = ArucoTools(685, 17.25, 30, "Camera Calibration/Calib2/Calibration.npy")
a3 = ArucoTools(685, 17.25, 30, "Camera Calibration/Calib5/Calibration.npy")
# a4 = ArucoTools(685, 17.25, 30, "Camera Calibration/Calib4/Calibration.npy")
# a5 = ArucoTools(685, 17.25, 30, "Camera Calibration/Calib5/Calibration.npy")

tello1 = TelloObject("10.3.141.101", 11112, [0,0,0], a1)
tello2 = TelloObject("10.3.141.102", 11113, [0,0,0], a2)
tello3 = TelloObject("10.3.141.105", 11114, [0,0,0], a3)
# tello4 = TelloObject("10.3.141.104", 11115, [0,0,0], a4)
# tello5 = TelloObject("10.3.141.105", 11116, [0,0,0], a5)

############################ START UP ################################

SC = SwarmControl([tello1, tello2, tello3])
SC.do_for_all("getBattery()")

sleep(2)

SC.do_for_all_in_threads("startCam")

sleep(2)

SC.do_for_all_in_threads("takeoff")

sleep(1)

############################ MOVE UP ################################

move_up_cm = 50

thread1 = threading.Thread(target=move_up, args=(tello1, move_up_cm), name="tello1 move up")
thread2 = threading.Thread(target=move_up, args=(tello2, move_up_cm), name="tello2 move up")
thread3 = threading.Thread(target=move_up, args=(tello3, move_up_cm), name="tello3 move up")

thread1.start()
thread2.start()
thread3.start()

thread1.join()
thread2.join()
thread3.join()

sleep(1)

############################ TARGET 1 ################################

a1._TargetID = 685
a2._TargetID = 685
a3._TargetID = 685

tello1._position = [-40, -30, -125]
tello2._position = [ 40, -30, -125]
tello3._position = [  0, -30, -190]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

sleep(1)

############################ ROTATE 120 CW ################################

tello1._position = [  0, -30, -100]
tello2._position = [ 40, -30, -165]
tello3._position = [-40, -30, -165]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

sleep(1)

tello1._position = [ 40, -30, -125]
tello2._position = [  0, -30, -190]
tello3._position = [-40, -30, -125]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

sleep(1)


############################ TARGET 2 ################################

a1._TargetID = 605
a2._TargetID = 605
a3._TargetID = 605

thread1 = threading.Thread(target=rotate_self, args=(tello1, 146), name="tello1 rotate")
thread2 = threading.Thread(target=rotate_self, args=(tello2, 120), name="tello2 rotate")
thread3 = threading.Thread(target=rotate_self, args=(tello3,  94), name="tello3 rotate")

thread1.start()
thread2.start()
thread3.start()

thread1.join()
thread2.join()
thread3.join()

tello1._position = [-40, -30, -360]
tello2._position = [ 40, -30, -360]
tello3._position = [  0, -30, -425]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

sleep(1)

tello1._position = [-40, -30, -230]
tello2._position = [ 40, -30, -230]
tello3._position = [  0, -30, -295]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

sleep(1)

tello1._position = [-40, -30, -100]
tello2._position = [ 40, -30, -100]
tello3._position = [  0, -30, -165]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

############################ ROTATE 180 CW ################################

tello1._position = [  0, -30, -100]
tello2._position = [ 40, -30, -165]
tello3._position = [-40, -30, -165]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

sleep(1)

tello1._position = [ 40, -30, -125]
tello2._position = [  0, -30, -190]
tello3._position = [-40, -30, -125]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

sleep(1)

tello1._position = [ 40, -30, -165]
tello2._position = [-40, -30, -165]
tello3._position = [  0, -30, -100]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

sleep(1)

############################ TARGET 3 ################################

a1._TargetID = 625
a2._TargetID = 625
a3._TargetID = 625

thread1 = threading.Thread(target=rotate_self, args=(tello1, 190), name="tello1 rotate")
thread2 = threading.Thread(target=rotate_self, args=(tello2, 170), name="tello2 rotate")
thread3 = threading.Thread(target=rotate_self, args=(tello3, 180), name="tello3 rotate")

thread1.start()
thread2.start()
thread3.start()

thread1.join()
thread2.join()
thread3.join()

tello1._position = [-40, -30, -750]
tello2._position = [ 40, -30, -750]
tello3._position = [  0, -30, -815]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

tello1._position = [-40, -30, -650]
tello2._position = [ 40, -30, -650]
tello3._position = [  0, -30, -715]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

tello1._position = [-40, -30, -550]
tello2._position = [ 40, -30, -550]
tello3._position = [  0, -30, -615]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

tello1._position = [-40, -30, -450]
tello2._position = [ 40, -30, -450]
tello3._position = [  0, -30, -515]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

tello1._position = [-40, -30, -350]
tello2._position = [ 40, -30, -350]
tello3._position = [  0, -30, -415]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

tello1._position = [-40, -30, -250]
tello2._position = [ 40, -30, -250]
tello3._position = [  0, -30, -315]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

tello1._position = [-40, -30, -125]
tello2._position = [ 40, -30, -125]
tello3._position = [  0, -30, -190]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

############################ ROTATE 120 CCW ################################

tello1._position = [-40, -30, -100]
tello2._position = [  0, -30, -165]
tello3._position = [ 40, -30, -165]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

sleep(1)

tello1._position = [  0, -30, -190]
tello2._position = [-40, -30, -125]
tello3._position = [ 40, -30, -125]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

sleep(1)


############################ TARGET 4 ################################

a1._TargetID = 665
a2._TargetID = 665
a3._TargetID = 665

thread1 = threading.Thread(target=rotate_self, args=(tello1, 240), name="tello1 rotate")
thread2 = threading.Thread(target=rotate_self, args=(tello2, 214), name="tello2 rotate")
thread3 = threading.Thread(target=rotate_self, args=(tello3, 266), name="tello3 rotate")

thread1.start()
thread2.start()
thread3.start()

thread1.join()
thread2.join()
thread3.join()

tello1._position = [-40 + 100, -30, -360]
tello2._position = [ 40 + 100, -30, -360]
tello3._position = [  0 + 100, -30, -425]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

sleep(1)

tello1._position = [-40 + 50, -30, -230]
tello2._position = [ 40 + 50, -30, -230]
tello3._position = [  0 + 50, -30, -295]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

sleep(1)

tello1._position = [-40, -30, -100]
tello2._position = [ 40, -30, -100]
tello3._position = [  0, -30, -165]

keyPressed = 0
while keyPressed != ord("q") and keyPressed != ord("a"):
    # bias = tello4.trackLoop()
    tello1.trackLoop()
    tello2.trackLoop()
    tello3.trackLoop()
    keyPressed = cv2.waitKey(50)

if keyPressed == ord("a"):
    SC.do_for_all_in_threads("kill")
    exit(1)

SC.do_for_all_in_threads("kill")

exit(0)
