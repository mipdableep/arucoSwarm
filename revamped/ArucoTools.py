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

    def __init__(self, calibPath, num, dist = 120):
        self._TargetID = -1
        self._TargetSize = -1
        self.num = num
        
        self.dist = dist
        
        fs = cv2.FileStorage(calibPath, cv2.FILE_STORAGE_READ)

        self._camMatrix = fs.getNode("camera_matrix").mat()
        self._camDist = fs.getNode("distortion_coefficients").mat()
        
        if self._camMatrix is None:
            self._camMatrix = fs.getNode("Camera_Matrix").mat()
            self._camDist = fs.getNode("Distortion_Coefficients").mat()
        


    def set_target(self, target_id, target_size):
        self._TargetID = target_id
        self._TargetSize = target_size

   
    def calculate_location(self, img:cv2.Mat):
        """
        gets R and T vecs from img

        Args:
            image (cv2.Mat): input frame

        Returns:
            status (int): status
            R : rotation vector
            T : translation vector
            img : img with drawn marker
        """
                
        markerCorners, markerIds, _ = detector.detectMarkers(img)
        # draw the markers
        cv2.aruco.drawDetectedMarkers(img, markerCorners, markerIds)

        if markerIds is None:
            return -9, None, None, img

        idFound = False
        targetIndex = -1

        for index, id in enumerate(markerIds):
            if id == self._TargetID:
                idFound = True
                targetIndex = index
        
        if not idFound:
            return -9, None, None, img
        
        rvecs, tvecs, _ = cv2.aruco.estimatePoseSingleMarkers(markerCorners[targetIndex], self._TargetSize, self._camMatrix, self._camDist)

        if rvecs is None or tvecs is None:
            return -9, None, None, img
        
        
        rvec = rvecs[0][0]

        tvec = tvecs[0][0]
        
        cv2.drawFrameAxes(img, self._camMatrix, self._camDist, rvec, tvec, 3.5, 2)

        R, T = self.extract_6_dof_from_vecs(rvec, tvec)
        return 0, R , T, img



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
        
        # print("1 ", tvec)
        tvec = (tvec * -rmat)
        # print("2 ", tvec)
        tvec = np.array(tvec[0])
        # print("3 ", tvec)
        
        
        pitch = euler_angles[0]
        if pitch > 0: 
            pitch = 180-pitch
        else:
            pitch = -pitch - 180
        yaw = euler_angles[1]
        roll = euler_angles[2]
        R = {"pitch":int(pitch), "yaw":int(yaw), "roll":int(roll)}
        T = {"lr":int(tvec[0][0]), "fb":int(tvec[0][2]), "ud":int(tvec[0][1])}
        
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
