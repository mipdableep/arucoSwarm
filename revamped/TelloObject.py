import cv2
import numpy as np
from djitellopy import Tello
import VCS
from ArucoTools import ArucoTools
import datetime as dt

class TelloObject:

    def __init__(self, address : str, vport : int, angle, distance, hight, arucoTool : ArucoTools, num, searchCW, isLeader = False, bias_dev = 1):
        # connection vals
        self._vport = vport
        self._address = address
        
        self.num = num
        self._isLeader = isLeader
        self.bias_dev = bias_dev
        
        self.target_reached = False
        
        # location vals
        self._angle = angle
        self._distance = distance
        self._hight = hight
        
        self._arucoTool = arucoTool
        self._FrameCounter = 0
        
        # no leader counter
        self._nlc = 0
        self._searchCW = searchCW
        self.si = 10
        
        self._tello = Tello(address)
        self._tello.connect()
        self._tello.set_network_ports(8890, vport)
        self._tello.set_video_bitrate(Tello.BITRATE_1MBPS)
        self._tello.streamon()
        
    
    def stop(self):
        self._tello.send_rc_control(0,0,0,0)
    
    def startCam(self):
        self.cam = VCS.VideoCapture("udp://" + self._address + ":" + str(self._vport))
        self.title = "TELLO-" + self._address

    def takeoff(self):
        self._tello.takeoff()
        pass
    
    def upup(self):
        self._tello.move_up(100)

    def streamOff(self):
        self._tello.streamoff()

    def kill(self):
        self._tello.send_rc_control(0,0,0,0)
        self._tello.land()
        self.cam.release()
        
    def getBattery(self):
        print(self.num, ": ", self._tello.get_battery())

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

        # save img to path
        filename = 'video/' + str(self.num) + '/' + str(self._FrameCounter) + '.jpg'
        cv2.imwrite(filename, img)

        self._FrameCounter += 1

        if status == -9 and self._isLeader:
            print ("target aruco not found")
            self._tello.send_rc_control(lr, fb, ud, cw)
            frame = cv2.resize(input_frame, (720, 480))
            cv2.imshow(self.title, frame)
            return lr, fb, ud

        elif status == -9:
            print ("target aruco not found")
            self._nlc += 1
            
            if self._nlc > self.si and self._nlc < 3 * self.si:
                self._tello.send_rc_control(lr, fb, ud, cw + self._searchCW)

            elif self._nlc > 3 * self.si and self._nlc < 7 * self.si:
                self._tello.send_rc_control(lr, fb, ud + 21, cw - self._searchCW)
            
            elif self._nlc > 7 * self.si and self._nlc < 9*self.si:
                self._tello.send_rc_control(lr, fb, ud, cw + self._searchCW)
                
            elif self._nlc > 9*self.si:
                self._tello.send_rc_control(lr, fb, ud, cw)
                self._nlc = 0
                
            else:
                self._tello.send_rc_control(lr, fb, ud, cw)
            
            frame = cv2.resize(input_frame, (720, 480))
            cv2.imshow(str(self.num), frame)
            
            return lr, fb, ud


        self._nlc = 0

        self._tello.send_rc_control(lr, fb, ud, cw)
        
        if status == 5:
            self.target_reached = True

        img = cv2.resize(img, (720, 480))
        cv2.imshow(self.title, img)

        return int(lr/self.bias_dev), int(fb/self.bias_dev), int(ud/self.bias_dev)

    
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
    

