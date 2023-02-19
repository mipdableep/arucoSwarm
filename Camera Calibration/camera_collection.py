import cv2
import numpy as np
from time import sleep

import threading

from djitellopy import Tello

tello1 = Tello("10.3.141.159")
tello1.connect()

tello1.set_network_ports(8890, 11113)
tello1.streamon()

webcam = cv2.VideoCapture('udp://10.3.141.159:11113?overrun_nonfatal=1&fifo_size=50')
webcam.set(cv2.CAP_PROP_BUFFERSIZE, 0)

ret, frame = webcam.read()

count = 0
# Chess board crosses
cross = (6, 9)
# termination criteria
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

while cv2.waitKey(1) != ord("q"):
    ret, frame = webcam.read()

    for _ in range(40):
        ret, frame = webcam.read()

    if not ret:
        print ('Error retriving video stream')
        break

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Find the chess board corners
    ret, corners = cv2.findChessboardCorners(gray, cross, None)

    # If found, add object points, image points (after refining them)
    if ret:
        corners2 = cv2.cornerSubPix(gray,corners, (11,11), (-1,-1), criteria)
        cv2.drawChessboardCorners(gray, cross, corners2, ret)
    
    cv2.imshow("Match", gray)

    if cv2.waitKey(1) == ord("s"):
        count = count + 1
        cv2.imwrite('/home/daniel/Documents/Camera Calibration/Images3/' + str(count) + '.jpg', frame)
        print ("scae")
        sleep(0.5)

webcam.release()