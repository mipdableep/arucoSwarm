import cv2
import numpy as np
from djitellopy import Tello
import VCS
from ArucoTools import ArucoTools
import datetime as dt

class TelloObject:

    def __init__(self, address : str, vport : int, angle, distance, hight, arucoTool : ArucoTools):
        # connection vals
        self._vport = vport
        self._address = address
        
        # location vals
        self._angle = angle
        self._distance = distance
        self._hight = hight
        
        self._arucoTool = arucoTool
        
        self._tello = Tello(address)
        self._tello.connect()
        self._tello.set_network_ports(8890, vport)
        self._tello.set_video_bitrate(Tello.BITRATE_1MBPS)
        self._tello.streamon()
        
    
    def startCam(self):
        self.cam = VCS.VideoCapture("udp://" + self._address + ":" + str(self._vport))
        self.title = "TELLO-" + self._address

    def takeoff(self):
        self._tello.takeoff()
        pass

    def kill(self):
        self._tello.streamoff()
        self._tello.land()
        self.cam.release()
        
    def getBattery(self):
        print(self._tello.get_battery())

    def trackLoop(self, bias=[0,0,0]):

        ret, input_frame = self.cam.read()

        if not ret:
            print ('Error retriving video stream')
            return lr, fb, ud
    
        status, lr, fb, ud, cw, img = self._arucoTool.arucofunc(input_frame, self._distance, self._angle, self._hight, bias)
        
        lr = int(lr)
        fb = int(fb)
        ud = int(ud)
        cw = int(cw)

        if status == -9:
            print ("target aruco not found")
            self._tello.send_rc_control(lr, fb, ud, cw)
            frame = cv2.resize(input_frame, (720, 480))
            cv2.imshow(self.title, frame)
            return lr, fb, ud

        self._tello.send_rc_control(lr, fb, ud, cw)
        filename = './arucos/' + self._address + '/' + dt.datetime.now().strftime("%Y%m%d_%H:%M:%S:%f") + '.jpg'

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

        cv2.imwrite(filename, img)

        img = cv2.resize(img, (720, 480))
        cv2.imshow(self.title, img)

        return lr, fb, ud

    
    # Util -> polar coordinate to rectangular coordinate
    def rect (angle, radius):
        return np.array([radius * np.cos(angle), radius * np.sin(angle)])


    def rotate (self, angle, radius, arc, speed):

        angle, arc = np.deg2rad(angle), np.deg2rad(arc)

        # Center of the circle of rotation
        pivot = self.rect(angle, radius)

        # Calculate midpoint (p1) and endpoint (p2)
        p1 = pivot + self.rect (-np.pi + arc/2 + angle, radius)
        p2 = pivot + self.rect (-np.pi +  arc  + angle, radius)

        self._tello.curve_xyz_speed(int(p1[0]), int(p1[1]), 0, int(p2[0]), int(p2[1]), 0, speed)
    

