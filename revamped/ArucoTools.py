import cv2
import numpy as np

# Tello camera points 12 degree downwards
cameraFixedAngle = 12

# setup
dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_ARUCO_ORIGINAL)
# Set detection parameters
detectorParams = cv2.aruco.DetectorParameters()
# Setup Aruco detector
detector = cv2.aruco.ArucoDetector(dictionary, detectorParams)

class ArucoTools:

    def __init__(self, TargetID, TargetSize, calibPath):
        self._TargetID = TargetID
        self._TargetSize = TargetSize
        
        with open(calibPath, 'rb') as f:
            self._camMatrix = np.load(f)
            self._camDist = np.load(f)
        
        self._CLIP = 30
        self._YAW_CLIP = 100
        self._directionCalib = {"LR" : -0.5,
                                "FB" : -0.5,
                                "UD" : -2.0,
                                "CW" :  2.5 }

    def arucofunc(self, img:cv2.Mat, distance, angle, bias):
        # Detect the arucos in the image
        markerCorners, markerIds, _ = detector.detectMarkers(img)
        # draw the markers
        cv2.aruco.drawDetectedMarkers(img, markerCorners, markerIds)
        
        if markerIds is None:
            return -9, 0, 0, 0, 0, 0

        idFound = False
        targetIndex = -1

        for index, id in enumerate(markerIds):
            if id == self._TargetID:
                idFound = True
                targetIndex = index
        
        if not idFound:
            return -9, 0, 0, 0, 0, 0
        
        rvecs, tvecs, _ = cv2.aruco.estimatePoseSingleMarkers(markerCorners[targetIndex], self._TargetSize, self._camMatrix, self._camDist)

        if rvecs is None:
            return -9, 0, 0, 0, 0, 0
        
        rvec = rvecs[0][0]
        tvec = tvecs[0].transpose()
        
        # Drae 3D axes on the aruco to understand better thier pose. (3.5 cm - axes length, 2 - axes weidth)
        cv2.drawFrameAxes(img, self._camMatrix, self._camDist, rvec, tvec, 3.5, 2)

        rmat, _ = cv2.Rodrigues(rvec)
        rmat = np.matrix(rmat)

        zvec = np.matrix([[0.], [0.], [-1.]])
        pvec = rmat * zvec

        px = pvec[0,0]
        py = pvec[1,0]
        pz = pvec[2,0]
        
        cx = tvec[0,0]
        cy = tvec[1,0]
        cz = tvec[2,0]

        # get vlues
        pyaw = R2D(np.arctan2(px, pz))
        prange = np.sqrt(cx ** 2 + cy ** 2 + cz ** 2)

        cpitch_bais = np.sqrt(cy ** 2 + cz ** 2) * np.sin(D2R(cameraFixedAngle))
        cyaw = R2D(np.arctan2(cx, cz))

        # directional commands and vals
        lr = self._directionCalib["LR"] * (pyaw - cyaw - angle)
        fb = self._directionCalib["FB"] * (distance - prange)
        ud = self._directionCalib["UD"] * (cy + cpitch_bais)
        cw = self._directionCalib["CW"] * (cyaw)
        
        for i in [lr, fb, ud]:
            i = np.clip(i, -self._CLIP, self._CLIP)
        
        cw = np.clip(cw, -self._YAW_CLIP, self._YAW_CLIP)
        
        return 0, lr, fb, ud, cw, img


def R2D(radians):
    """
    radians to degrees

    Args:
        radians (float) : input
    """        
    return radians * 180. / np.pi
    
def D2R(degree):
    """
    degree to radians

    Args:
        degree (float): input
    """
    return degree * np.pi / 180.
