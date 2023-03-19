import cv2
import numpy as np
from scipy.spatial.transform import Rotation


# Tello camera points 12 degree downwards
cameraFixedAngle = 12

# setup
dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_ARUCO_ORIGINAL)
# Set detection parameters
detectorParams = cv2.aruco.DetectorParameters()
# Setup Aruco detector
detector = cv2.aruco.ArucoDetector(dictionary, detectorParams)

class ArucoTools:

    def __init__(self, TargetID, TargetSize, clip, calibPath, num, dist = 120):
        self._TargetID = TargetID
        self._TargetSize = TargetSize
        self.num = num
        
        self.dist = dist
        
        fs = cv2.FileStorage(calibPath, cv2.FILE_STORAGE_READ)

        self._camMatrix = fs.getNode("camera_matrix").mat()
        self._camDist = fs.getNode("distortion_coefficients").mat()

        self._CLIP = clip
        self._YAW_CLIP = 100
        self._directionCalib = {"LR" : -0.5,
                                "FB" : -0.5,
                                "UD" : -2.0,
                                "CW" :  2.0 }


   
    def calculate_location(self, image:cv2.Mat):
        img = image.copy()
        
        markerCorners, markerIds, _ = detector.detectMarkers(img)
        # draw the markers
        cv2.aruco.drawDetectedMarkers(img, markerCorners, markerIds)

        if markerIds is None:
            return -9, None, None, image

        idFound = False
        targetIndex = -1

        for index, id in enumerate(markerIds):
            if id == self._TargetID:
                idFound = True
                targetIndex = index
        
        if not idFound:
            return -9, None, None, image
        
        rvecs, tvecs, _ = cv2.aruco.estimatePoseSingleMarkers(markerCorners[targetIndex], self._TargetSize, self._camMatrix, self._camDist)

        if rvecs is None:
            return -9, None, None, image
        
        
        rvec = rvecs[0][0]
        # print("\n\n\n", rvec)

        tvec = tvecs[0]
        
        cv2.drawFrameAxes(img, self._camMatrix, self._camDist, rvec, tvec, 3.5, 2)

        R, T = self.extract_6_dof_from_vecs(rvec, tvec)
        return 0, R , T, img
        

    def arucofunc(self, image:cv2.Mat, distance, angle, hight):
        # Detect the arucos in the image
        img = image.copy()
        
        markerCorners, markerIds, _ = detector.detectMarkers(img)
        # draw the markers
        cv2.aruco.drawDetectedMarkers(img, markerCorners, markerIds)
        
        lr, fb, ud, cw = 0, 0, 0, 0

        if markerIds is None:
            return -9, lr, fb, ud, cw, image

        idFound = False
        targetIndex = -1

        for index, id in enumerate(markerIds):
            if id == self._TargetID:
                idFound = True
                targetIndex = index
        
        if not idFound:
            return -9, lr, fb, ud, cw, image
        
        rvecs, tvecs, _ = cv2.aruco.estimatePoseSingleMarkers(markerCorners[targetIndex], self._TargetSize, self._camMatrix, self._camDist)

        if rvecs is None:
            return -9, lr, fb, ud, cw, image
        
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
        lr = self._directionCalib["LR"] * ((pyaw - angle))
        fb = self._directionCalib["FB"] * (distance - prange)
        ud = self._directionCalib["UD"] * (hight + cy + cpitch_bais)
        cw = self._directionCalib["CW"] * (cyaw)
        
        lr = np.clip(lr, -self._CLIP, self._CLIP)
        fb = np.clip(fb, -self._CLIP, self._CLIP)
        ud = np.clip(ud, -self._CLIP, self._CLIP)
        
        cw = np.clip(cw, -self._YAW_CLIP, self._YAW_CLIP)
        
        self.draw_img(img, lr, fb, ud, cw, prange, pyaw)
        
        if prange < self.dist:
            return 5, lr, fb, ud, cw, img
        else:
            return 0, lr, fb, ud, cw, img

    def extract_6_dof_from_vecs(self, rvec, tvec):
        """
        extract 6 dof into euler angles and dist dicts

        Args:
            rvec : rotation vector
            tvec : translation vector

        Returns:
            rotation, translation dicts
        """
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
        # TODO: check if the axis are correct
        R = {"pitch":pitch, "yaw":yaw, "roll":roll}
        T = {"lr":tvec[0], "fb":tvec[1], "ud":tvec[2]}
        
        return R, T

    def draw_img(self, img, lr, fb, ud, cw, prange, pyaw):
    
    # font
        font = cv2.FONT_HERSHEY_SIMPLEX
        fontScale = int(1)
        color = (0, 0, 255)
        thickness = int(2)
        
        # Add text
        img = cv2.putText(img, ("L/R : " + str(lr)), (10,  30), font, fontScale, color, thickness, cv2.LINE_AA)
        img = cv2.putText(img, ("F/B : " + str(fb)), (10,  60), font, fontScale, color, thickness, cv2.LINE_AA)
        img = cv2.putText(img, ("U/D : " + str(ud)), (10,  90), font, fontScale, color, thickness, cv2.LINE_AA)
        img = cv2.putText(img, ("CCW : " + str(cw)), (10, 120), font, fontScale, color, thickness, cv2.LINE_AA)        
        img = cv2.putText(img, ("dist: " + str(prange)), (10, 180), font, fontScale, color, thickness, cv2.LINE_AA)
        img = cv2.putText(img, ("angle: " + str(pyaw)), (10, 210), font, fontScale, color, thickness, cv2.LINE_AA)
    

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
