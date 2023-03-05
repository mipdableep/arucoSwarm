import numpy as np
import cv2 as cv

cam = cv.VideoCapture(0)

while cv.waitKey(10) != ord("q"):
    ret, frame = cam.read()

    cv.putText(frame, "Text", (10, 30), )
    cv.imshow("img", frame)

cam.release()