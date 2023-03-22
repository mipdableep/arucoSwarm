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

    def __init__(self, clip, calibPath):
        self._TargetID = -1
        self._TargetSize = -1
        
        with open(calibPath, 'rb') as f:
            self._camMatrix = np.load(f)
            self._camDist = np.load(f)
        
        self._CLIP = clip
        self._YAW_CLIP = 100
        self._directionCalib = {"LR" :  0.4,
                                "FB" :  0.4,
                                "UD" :  1.0,
                                "CW" :  2.0 }
    
    def set_target(self, target_id, target_size):
        self._TargetID = target_id
        self._TargetSize = target_size

    def rebias (self, bias, angle):
        [b_lr, b_fb, b_ud] = bias

        lr = b_lr * np.cos(angle) - b_fb * np.sin(angle)
        fb = b_lr * np.sin(angle) + b_fb * np.cos(angle)
        ud = b_ud

        return [lr, fb, ud]

    def arucofunc(self, img:cv2.Mat, position : np.array, bias):
        # Detect the arucos in the image
        markerCorners, markerIds, _ = detector.detectMarkers(img)
        # draw the markers
        cv2.aruco.drawDetectedMarkers(img, markerCorners, markerIds)
        
        lr, fb, ud = bias
        cw = 0

        if markerIds is None:
            return -9, lr, fb, ud, cw, 0

        idFound = False
        targetIndex = -1

        for index, id in enumerate(markerIds):
            if id == self._TargetID:
                idFound = True
                targetIndex = index
        
        if not idFound:
            return -9, lr, fb, ud, cw, 0
        
        rvecs, tvecs, _ = cv2.aruco.estimatePoseSingleMarkers(markerCorners[targetIndex], self._TargetSize, self._camMatrix, self._camDist)

        if rvecs is None:
            return -9, lr, fb, ud, cw, 0
        
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

        yaw = np.arctan2(px, pz)

        mx = -cx
        my = -cy - cpitch_bais
        mz = -cz

        tx = position[0] * np.cos(-yaw) - position[2] * np.sin(-yaw)
        ty = position[1]
        tz = position[0] * np.sin(-yaw) + position[2] * np.cos(-yaw)

        self.error = np.sqrt((tx - mx)**2 + (ty - my)**2 + (tz - mz)**2)
        self.pos = [mx * np.cos(yaw) - mz * np.sin(yaw),
                    my,
                    mx * np.sin(yaw) + mz * np.cos(yaw)]

        if cyaw > 10:
            cyaw = 2 * cyaw - 10

        # directional commands and vals
        lr = self._directionCalib["LR"] * (tx - mx)
        fb = self._directionCalib["FB"] * (tz - mz)
        ud = self._directionCalib["UD"] * (my - ty)
        cw = self._directionCalib["CW"] * (cyaw)

        xy_norm = np.sqrt(lr**2 + fb**2)

        if (xy_norm > self._CLIP):
            xy_clip = self._CLIP / xy_norm
            lr = lr * xy_clip
            fb = fb * xy_clip

        lr = np.clip(int(lr), -self._CLIP, self._CLIP)
        fb = np.clip(int(fb), -self._CLIP, self._CLIP)
        ud = np.clip(int(ud), -self._CLIP, self._CLIP)
        
        cw = np.clip(cw, -self._YAW_CLIP, self._YAW_CLIP)

        '''
        print ('--------------------------------------')
        print ('LR : ' + str(lr))
        print ('FB : ' + str(fb))
        print ('UD : ' + str(ud))
        print ('CCW: ' + str(cw))
        '''
        
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
