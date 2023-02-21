import cv2
import numpy as np
from time import sleep
from datetime import datetime
from VCS import VideoCaptureSpeeder

webcam = VideoCaptureSpeeder(0)

# Save folder
folder = '/home/daniel/Documents/arucoSwarm/Camera Calibration/Images4'
# Count number of pictures
count = 0
# Chess board crosses
cross = (7, 7)
# termination criteria
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

while True:
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
    
    img = cv2.resize(gray, (720, 480))
    cv2.imshow("Match", img)

    key = cv2.waitKey(1)

    if key == ord("s"):
        count = count + 1
        current_time = datetime.now().strftime("%Y%m%d_%H:%M:%S")
        cv2.imwrite(folder + '/' + str(current_time) + '.jpg', frame)
        print ("saved - " + str(current_time) + " - " + str(count))
        sleep(0.5)

    elif key == ord("q"):
        break
    


webcam.release()