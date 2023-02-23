import cv2
import numpy as np
from djitellopy import Tello
import VCS
import TelloUtils
from time import sleep
import threading

'''
# Laptop camera calibration information
camMatrix = np.array([[642.70447427,   0.        , 341.57956398],
                      [  0.        , 637.75160549, 233.37420422],
                      [  0.        ,   0.        ,   1.        ]])
camDistortion = np.array([[ 0.03411031, -0.10003022,  0.00066265,  0.00281555, -0.10130354]])
'''
'''
#1280x720
# Drone calibration
camMatrix = np.array([[960.62263735,   0.        , 470.52922775],
                      [  0.        , 963.94910764, 331.7355087 ],
                      [  0.        ,   0.        ,   1.        ]])
camDistortion = np.array([[ 2.39342314e-02, -2.66942206e-01, -8.06335405e-03,  2.31336119e-04,  8.11008436e-01]]) 
'''
#1280 x 720
camMatrix = np.array(
    [[924.49497254,   0.        , 479.20422298],
     [  0.        , 926.01552365, 366.64431926],
     [  0.        ,   0.        ,   1.        ]])
camDistortion = np.array([[-0.00276759,  0.09178107,  0.00083479, -0.00039963, -0.12152757]])

# Aruco square size in cm
aruco_size = 7.0


# Detect all arucos (that in dictionary) in the given image
def detectAruco (inputImage):
    # Set Aruco dictionary
    dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_ARUCO_ORIGINAL)
    # Set detection parameters
    detectorParams = cv2.aruco.DetectorParameters()
    # Setup Aruco detector
    detector = cv2.aruco.ArucoDetector(dictionary, detectorParams)

    # Detect the arucos in the image
    markerCorners, markerIds, _ = detector.detectMarkers(inputImage)

    # Return the markers position, and thier coresponding decoded IDs
    return markerCorners, markerIds


# Draw box around the detected arucos
def drawMarkers (inputImage, markerCorners, markerIds=None):
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


# Draw Grid lines on image
def drawGrid (inputImage : np.array, v_lines : int, h_lines : int, color=(0, 0, 0)):
    # Create new image to not overwrite the original
    outputImage = np.copy(inputImage)

    h, w, _ = outputImage.shape

    # Print horizontal lines
    for hline in range(0, w, int(w / (h_lines + 1))):
        outputImage = cv2.line(outputImage, (hline, 0), (hline, h), color)
    
    # Print vertical lines
    for vline in range(0, h, int(h / (v_lines + 1))):
        outputImage = cv2.line(outputImage, (0, vline), (w, vline), color)

    return outputImage

class TelloObject:

    def __init__(self, address : str, vport : int):
        self._vport = vport
        self._address = address
        self._tello = Tello(address)
        self._tello.connect()
        self._tello.set_network_ports(8890, vport)
        self._tello.set_video_bitrate(Tello.BITRATE_1MBPS)
        self._tello.streamon()
        return
    
    def takeoff(self):
        self._tello.takeoff()
        return
    
    def startCam(self):
        self.cam = VCS.VideoCapture("udp://" + self._address + ":" + str(self._vport))
        self.title = "TELLO-" + self._address

    def track(self, arucoID : int, angle : float, distance : float):

        self.ret, self.frame = self.cam.read()

        if not self.ret:
            print ('Error retriving video stream')
            return

        # Detect arucos and extract thier positions
        markerCorners, markerIds = detectAruco(self.frame)
        rvecs, tvecs = extractArucoPosition(markerCorners)

        # Mark the arucos and draw thier pose
        self.img = np.copy (self.frame)

        self.img = drawMarkers (self.img, markerCorners)
        #img = drawMarkers (img, markerCorners, markerIds)
        self.img = drawAxes (self.img, rvecs, tvecs)
        self.img = drawGrid (self.img, 5, 5, (255, 255, 255))
        self.img = cv2.flip (self.img, 1)

        lr, fb, ud, cw = 0, 0, 0, 0

        # Print all the positions
        for i in range(len(rvecs)):

            if markerIds[i] != arucoID:
                continue

            rvec = rvecs[i][0]
            tvec = tvecs[i].transpose()

            rmat, _ = cv2.Rodrigues(rvec)
            rmat = np.matrix(rmat)

            zvec = np.matrix([[0.], [0.], [-1.]])
            pvec = rmat * zvec

            px, py, pz = TelloUtils.extractVector(pvec)
            cx, cy, cz = TelloUtils.extractVector(tvec)

            pyaw = np.arctan2(px, pz) * 180. / np.pi
            ppitch = np.arctan2(py, np.sqrt(px ** 2 + pz ** 2)) * 180. / np.pi
            prange = np.sqrt(np.sum(tvecs[i][0] ** 2))

            ud = TelloUtils.fixValue(-2.0 * cy)
            cw = TelloUtils.fixValue( 150 * np.arctan2(cx, cz), 100)
            lr = TelloUtils.fixValue(-0.5 * (pyaw - angle))
            fb = TelloUtils.fixValue(-0.5 * (distance - prange))

            if False:
                print ("--------------------------------------------------")
                print ("Up/Down : " + str(ud))
                print ("Forward/Backword : " + str(fb))
                print ("Left/Right : " + str(lr))
                print ("ClockWise Rotation : " + str(cw))

                print ("--------------------------------------------------")
                print ("Yaw : " + str(np.around(pyaw, 2)) + "\xb0")
                print ("Pitch : " + str(np.around(ppitch, 2)) + "\xb0")
                print ("Distance : " + str(np.around(prange, 0)) + "cm")
                print ("Position : " + str(tvec.transpose()))

        self._tello.send_rc_control(lr, fb, ud, cw)
        self.img = cv2.resize(self.img, (720, 480))
        cv2.imshow(self.title, self.img)

    def kill(self):
        self._tello.streamoff()
        self._tello.land()


def takeoff_func(tello : TelloObject):
    tello.takeoff()

def startCam_func(tello : TelloObject):
    tello.startCam()

def kill_func(tello : TelloObject):
    tello.kill()




'''
!!!!!!!!!!! TOP VIEW !!!!!!!!!!!

         DRONE 3
            |
DRONE 1     |     DRONE 2
        \   |   /
          TARGET
'''

tello1 = TelloObject("10.3.141.169", 11112)
tello2 = TelloObject("10.3.141.67", 11113)
tello3 = TelloObject("10.3.141.117", 11114)

print (tello1._tello.get_battery())
print (tello2._tello.get_battery())
print (tello3._tello.get_battery())

tello1.startCam()
tello2.startCam()
tello3.startCam()

sleep(2)

thread1 = threading.Thread(target=takeoff_func, args=(tello1,), name="tello1 takeoff")
thread2 = threading.Thread(target=takeoff_func, args=(tello2,), name="tello2 takeoff")
thread3 = threading.Thread(target=takeoff_func, args=(tello3,), name="tello3 takeoff")

thread1.start()
thread2.start()
thread3.start()

thread1.join()
thread2.join()
thread3.join()

while cv2.waitKey(1) != ord("q"):
    tello1.track(145,  45,  70)
    tello2.track(145, -45,  70)
    tello3.track(145,   0, 100)


thread1 = threading.Thread(target=kill_func, args=(tello1,), name="tello1 kill")
thread2 = threading.Thread(target=kill_func, args=(tello2,), name="tello2 kill")
thread3 = threading.Thread(target=kill_func, args=(tello3,), name="tello3 kill")

thread1.start()
thread2.start()
thread3.start()

thread1.join()
thread2.join()
thread3.join()
