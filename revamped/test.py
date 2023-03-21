import cv2
from scipy.spatial.transform import Rotation
import numpy as np
from ArucoTools import ArucoTools

cap = cv2.VideoCapture()
cap.open(0)

# setup
dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_ARUCO_ORIGINAL)
# Set detection parameters
detectorParams = cv2.aruco.DetectorParameters()
# Setup Aruco detector
detector = cv2.aruco.ArucoDetector(dictionary, detectorParams)
run = True

a1 = ArucoTools("/home/fares/rbd/tools/calib/webcam.yaml", 1)
a1.set_target(685, 20)

while run:
    _, image = cap.read()
    if not _:
        continue
    img = image.copy()
    
    status, R, T, img = a1.calculate_location(img)
    
    if status == -9:
        image = cv2.resize(image, (720, 480))
        cv2.imshow("1", image)
        cv2.waitKey(20)
        continue
    
    print ("R:  ", R, "   T:  ", T)
    # print ("T:  ", T)
    image = cv2.resize(img, (720, 480))
    cv2.imshow("1", img)
    cv2.waitKey(20)
    

exit()
while (run):

    key = cv2.waitKey(25)
    if key == ord('q') or key == 27:
        run = False

    _, img = cap.read()
    if not _:
        continue
    image = img.copy()
    markerCorners, markerIds, _ = detector.detectMarkers(img)
    # draw the markers
    cv2.aruco.drawDetectedMarkers(img, markerCorners, markerIds)
    
    if markerIds is None:
        cv2.imshow("1", image)
        continue

    idFound = False
    targetIndex = -1

    for index, id in enumerate(markerIds):
        if id == 86:
            idFound = True
            targetIndex = index
    
    if not idFound:
        cv2.imshow("1", image)
        continue
    
    rvecs, tvecs, _ = cv2.aruco.estimatePoseSingleMarkers(markerCorners[targetIndex], 10, camMatrix, camDist)

    if rvecs is None:
        cv2.imshow("1", image)
        continue
    
    rvec = rvecs[0][0]
    # print("\n\n\n", rvec)

    tvec = tvecs[0]
    
    cv2.drawFrameAxes(img, camMatrix, camDist, rvec, tvec, 3.5, 2)
    cv2.imshow("1", img)


    rmat, _ = cv2.Rodrigues(rvec)
    rmat = np.matrix(rmat)
    euler_angles = Rotation.from_matrix(rmat).as_euler("xyz", degrees=True)
    
    pitch = euler_angles[0]
    if pitch > 0: 
        pitch = 180-pitch
    else:
        pitch = -pitch - 180
    yaw = euler_angles[1]
    roll = euler_angles[2]
    
    print (" pitch: ",pitch, " yaw: ",yaw)