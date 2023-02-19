# Capture video from Web-Camera using Python OpenCV
import cv2
import funcs
import numpy as np

webcam = cv2.VideoCapture(0)
webcam.set(cv2.CAP_PROP_BUFFERSIZE, 0)

while cv2.waitKey(1) != ord("q"):
    ret, frame = webcam.read()

    frame_h = np.size(frame, axis=0)
    frame_w = np.size(frame, axis=1)

    frame = frame[:,-1:0:-1]    # Mirror flip the image
    frame = cv2.resize(frame, dsize=(int(frame_w / 2), int(frame_h / 2)))
    
    if not ret:
        print ('Error retriving video stream')
        break

    gray = funcs.toGray(frame)

    blnc = funcs.balanceColors(frame, 1, 1, 0.8)

    hitmap = funcs.findPic(gray)

    find_map = np.zeros(hitmap.shape)
    np.max(hitmap, out=find_map)

    cv2.imshow("Webcam Stream", frame)
    cv2.imshow("Webcam - balance", blnc)
    cv2.imshow("Webcam - hitmap", hitmap)

webcam.release()
cv2.destroyAllWindows()