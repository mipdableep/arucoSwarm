import math
import numpy as np
import cv2
from scipy.spatial.transform import Rotation
import djitellopy
from VCS import VideoCapture
from time import sleep

rc_max = 18

class aruco_calc:
    
    X_pos = 0
    Y_pos = 0
    Z_pos = 0
    ZR_pos = 0
    current_wanted_Zr = 0
    
    
    def __init__(self, Xt, Yt, Zt, RZt) -> None:
        """set target vals

        Args:
            Xt: X target 
            Yt: Y target 
            Zt: Z target 
            RZt: RZ target 
        """
        self.X_TARGET = Xt
        self.Y_TARGET = Yt
        self.Z_TARGET = Zt
        self.RZ_TARGET = math.degrees(math.atan2(Xt, Yt))
    
    def calculate_rc_y(self):
        return int(np.clip((self.Y_TARGET - self.Y_pos)/-3, -rc_max, rc_max))
    
    def calculate_rc_z(self):
        return int(np.clip((self.Z_TARGET - self.Z_pos)/2, -rc_max, rc_max))
    
    def calculate_current_wanted_Zr(self):
        self.current_wanted_Zr = math.degrees(math.atan2(self.X_pos, self.Y_pos))
    
    def calculate_x_rc(self):
        return int(np.clip((self.RZ_TARGET - self.current_wanted_Zr)/1.5, -rc_max, rc_max))
    
    def calculate_rc_Rz(self):
        return int(np.clip((self.ZR_pos - self.current_wanted_Zr)*1.3, -rc_max, rc_max))
    
    def calculate_rc(self):

        self.calculate_current_wanted_Zr()
        print(self.current_wanted_Zr, "   ", self.ZR_pos)
        return (self.calculate_x_rc(), self.calculate_rc_y(), self.calculate_rc_z(), self.calculate_rc_Rz())
    
    """
    given r,t:
    rvecs, tvecs, _ = cv2.aruco.estimatePoseSingleMarkers(markerCorners[targetIndex], self._TargetSize, self._camMatrix, self._camDist)
    """

    def extract_6_dof_from_vecs(self, rvec, tvec):

        rmat, _ = cv2.Rodrigues(rvec)
        rmat = np.matrix(rmat)
        euler_angles = Rotation.from_matrix(rmat).as_euler("xyz", degrees=True)
        
        tvec = (tvec * -rmat)
        tvec = np.array(tvec[0])
        
        self.X_pos = int(tvec[0][0])
        self.Y_pos = int(tvec[0][2])
        self.Z_pos = int(tvec[0][1])
        self.ZR_pos = euler_angles[1]
        
        
        # ! uncomment to get full R,T vecs
        
        # pitch = euler_angles[0]
        # if pitch > 0: 
        #     pitch = 180-pitch
        # else:
        #     pitch = -pitch - 180
        # yaw = euler_angles[1]
        # roll = euler_angles[2]
        # R = {"pitch":int(pitch), "yaw":int(yaw), "roll":int(roll)}
        # T = {"lr":int(tvec[0][0]), "fb":int(tvec[0][2]), "ud":int(tvec[0][1])}
        
        # return R, T




def main():
    t = djitellopy.Tello()
    t.connect()
    t.streamon()
    t.send_rc_control(0,0,0,0)
    
    calc = aruco_calc(0, 70, 10, 0)


    fs = cv2.FileStorage("/home/fares/rbd/tools/calib/drone1.yaml", cv2.FILE_STORAGE_READ)

    camMatrix = fs.getNode("camera_matrix").mat()
    camDist = fs.getNode("distortion_coefficients").mat()

    if camMatrix is None:
        camMatrix = fs.getNode("Camera_Matrix").mat()
        camDist = fs.getNode("Distortion_Coefficients").mat()
        
    dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_ARUCO_ORIGINAL)
    detectorParams = cv2.aruco.DetectorParameters()
    detector = cv2.aruco.ArucoDetector(dictionary, detectorParams)


    cap = VideoCapture("udp://0.0.0.0:11111")
    
    t.takeoff()
    print("aa")
    markerCorners, markerIds = (), ()
    while True:
        ret, frame = cap.read()
        if ret != True:
            continue

        markerCorners, markerIds, _ = detector.detectMarkers(frame)
        
        cv2.aruco.drawDetectedMarkers(frame, markerCorners, markerIds)
        cv2.imshow("frame", frame)

        if markerIds is None:
            continue
        
        idFound = False
        targetIndex = -1

        for index, id in enumerate(markerIds):
            if id == 220:
                idFound = True
                targetIndex = index
        if not idFound:
            continue

        rvecs, tvecs, _ = cv2.aruco.estimatePoseSingleMarkers(markerCorners[targetIndex], 7, camMatrix, camDist)

        rvec = rvecs[0][0]
        tvec = tvecs[0][0]

        calc.extract_6_dof_from_vecs(rvec, tvec)
        x, y, z, zr = calc.calculate_rc()
        # print (f"x:{x}, y:{y}, z:{z}, zr:{zr}")
        t.send_rc_control(x, y, z, zr)
        
        cv2.waitKey(5)
        sleep(0.1)
        
if __name__ == "__main__":
    main()