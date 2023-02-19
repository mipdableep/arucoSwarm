import cv2
import numpy as np
from djitellopy import Tello
'''
# Laptop camera calibration information
camMatrix = np.array([[642.70447427,   0.        , 341.57956398],
                      [  0.        , 637.75160549, 233.37420422],
                      [  0.        ,   0.        ,   1.        ]])
camDistortion = np.array([[ 0.03411031, -0.10003022,  0.00066265,  0.00281555, -0.10130354]])
'''
#1280x720
camMatrix = np.array([[960.62263735,   0.        , 470.52922775],
                      [  0.        , 963.94910764, 331.7355087 ],
                      [  0.        ,   0.        ,   1.        ]])
camDistortion = np.array([[ 2.39342314e-02, -2.66942206e-01, -8.06335405e-03,  2.31336119e-04,  8.11008436e-01]]) 

# Aruco square size in cm
aruco_size = 7.0

# Detect all arucos (that in dictionary) in the given image
def detectAruco (inputImage):
    # Set Aruco dictionary
    dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_6X6_50)
    # Set detection parameters
    detectorParams = cv2.aruco.DetectorParameters()
    # Setup Aruco detector
    detector = cv2.aruco.ArucoDetector(dictionary, detectorParams)

    # Detect the arucos in the image
    markerCorners, markerIds, _ = detector.detectMarkers(inputImage)

    # Return the markers position, and thier coresponding decoded IDs
    return markerCorners, markerIds


# Draw box around the detected arucos
def drawMarkers (inputImage, markerCorners, markerIds):
    # Create new image to not overwrite the original
    outputImage = np.copy(inputImage)
    # Draw square around the markers
    cv2.aruco.drawDetectedMarkers(outputImage, markerCorners, markerIds)

    # Return the image with the marks
    return outputImage


# Extract position of the aruco in the image
def extractArucoPosition (markerCorners):
    # Estimate pose of each marker and return the values rvec and tvec---(different from those of camera coefficients)
    rvecs, tvecs, _ = cv2.aruco.estimatePoseSingleMarkers(markerCorners, aruco_size, camMatrix, camDistortion)
    
    # In case there are no arucos, its easier to work with empty arrays rether then 'NoneType'
    if rvecs is None:
        rvecs = []
        tvecs = []

    return rvecs, tvecs


# Draw axes of the arucos
def drawAxes (inputImage, rvecs, tvecs):
    # Create new image to not overwrite the original
    outputImage = np.copy(inputImage)

    # Iterate over all the found positions
    for i in range(len(rvecs)):
        # Drae 3D axes on the aruco to understand better thier pose. (3.5 cm - axes length, 2 - axes weidth)
        cv2.drawFrameAxes(outputImage, camMatrix, camDistortion, rvecs[i], tvecs[i], 3.5, 2)
    
    return outputImage


tello = Tello("10.3.141.159")
tello.connect()

tello.set_network_ports(8890, 11113)
tello.streamon()

tello.takeoff()

webcam = cv2.VideoCapture('udp://10.3.141.159:11113')
webcam.set(cv2.CAP_PROP_BUFFERSIZE, 0)

while cv2.waitKey(1) != ord("q"):

    for _ in range(5):
        webcam.read()

    ret, frame = webcam.read()

    if not ret:
        print ('Error retriving video stream')
        break

    # Detect arucos and extract thier positions
    markerCorners, markerIds = detectAruco(frame)
    rvecs, tvecs = extractArucoPosition(markerCorners)

    '''
    if len(rvecs) == 1:
        rvecs = np.array([[[0, 0, 0]]], float)
    '''

    # Mark the arucos and draw thier pose
    img = np.copy (frame)

    img = drawMarkers (img, markerCorners, markerIds)
    img = drawAxes (img, rvecs, tvecs)

    lr, fb, ud, cw = 0, 0, 0, 0

    # Print all the positions
    for i in range(len(rvecs)):
        rvec = rvecs[i][0]

        rmat, _ = cv2.Rodrigues(rvec)
        rmat = np.matrix(rmat)

        zvec = np.matrix([[0.], [0.], [-1.]])
        pvec = rmat * zvec

        px = pvec[0,0]
        py = pvec[1,0]
        pz = pvec[2,0]

        cx = tvecs[0,0,0]
        cy = tvecs[0,0,1]
        cz = tvecs[0,0,2]

        pyaw = np.arctan2(px, pz) * 180. / np.pi
        ppitch = np.arctan2(py, np.sqrt(px ** 2 + pz ** 2)) * 180. / np.pi
        distance = np.sqrt(np.sum(tvecs[i][0] ** 2))
        
        pyaw = np.around(pyaw, 2)
        ppitch = np.around(ppitch, 2)
        distance = np.around(distance, 1)

        if cy > 100:
            cy = 100
        elif cy < -100:
            cy = -100
        else:
            cy = int(cy)
        
        ud = -3 * cy

        if cx > 100:
            cx = 100
        elif cx < -100:
            cx = -100
        else:
            cx = int(cx)
        
        cw = 3 * cx / cz

        if distance > 110:
            fb = distance - 110
        elif distance < 90:
            fb = distance - 90
        
        fb /= 3

        if fb > 100:
            fb = 100
        elif fb < -100:
            fb = -100
        else:
            fb = int(fb)
        
        cw *= 10

        if cw > 100:
            cw = 100
        elif cw < -100:
            cw = -100
        else:
            cw = int(cw)

        print (ud)
        print (fb)
        print (lr)
        print (cw)

        print ("--------------------------------------------------")
        print ("Yaw : " + str(pyaw) + "\xb0")
        print ("Pitch : " + str(ppitch) + "\xb0")
        print ("Distance : " + str(distance) + "cm")
        #print ("Position : " + str(tvecs[i]))

    tello.send_rc_control(lr, fb, ud, cw)
    img = cv2.resize(img, (720, 480))
    cv2.imshow("Webcam out stream", img)

webcam.release()
