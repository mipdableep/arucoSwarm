import cv2
from djitellopy import Tello
import VCS
from ArucoTools import ArucoTools
import math
import numpy as np

class TelloObject:

    def __init__(self, address:str, vport:int, arucoTool:ArucoTools, num, ABS_POS, isLeader = False, startCam = True):
        # connection vals
        self._vport = vport
        self._address = address
        
        self.num = num
        
        self.target_reached = False
        self.isLeader = isLeader
        # location vals in relation to the middle point
        self._ABS_POS = ABS_POS
        
        self._last_target_detected = 0
        self._counter = 0
        
        self._arucoTool = arucoTool
        self._FrameCounter = 0
        self.yaw_dev = 2
        
        self._tello = Tello(address)
        self._tello.connect()
        self._tello.set_network_ports(8890, vport)
        self._tello.set_video_bitrate(Tello.BITRATE_1MBPS)
        
        if startCam:
            self._tello.streamon()
    
    def stop(self):
        self._tello.send_rc_control(0,0,0,0)
    
    def startCam(self):
        self.cam = VCS.VideoCapture("udp://" + self._address + ":" + str(self._vport))
        self.title = "TELLO-" + self._address
        self._tello.send_rc_control(0,0,0,0)

    def takeoff(self):
        self._tello.takeoff()
        pass

    def streamOff(self):
        self._tello.streamoff()

    def kill(self):
        self._tello.send_rc_control(0,0,0,0)
        self._tello.land()
        self.cam.release()
        
    def kill_no_cam(self):
        self._tello.send_rc_control(0,0,0,0)
        self._tello.land()
        
    def getBattery(self):
        print(self.num, ": ", self._tello.get_battery())

     
    def find_target(self):
        status, R, T = self.get_location(True)
        
        if status == -9 and self._last_target_detected > 5:
            self._tello.send_rc_control(0,0,0,40)
            self._last_target_detected += 1
            return False
        
        if status == -9:
            self._tello.send_rc_control(0,0,0,0)
            self._last_target_detected += 1
            return False
        
        self._last_target_detected = 0
        
        yaw = R["yaw"]
        lr = T["lr"]
        fb = T["fb"]
        
        wanted_yaw = math.atan2(lr, fb) * (180/math.pi)
    
        cw = int((wanted_yaw - yaw) * -1.7)
        
        self._tello.send_rc_control(0,0,0,cw)
        
        if abs(cw) < 5:
            self._counter += 1
        else:
            self._counter = 0
        
        print (self.num, " counter:  ", self._counter)
        
        if self._counter > 20:
            return True
        else:
            return False
        


    def get_location(self, imshow = False):
        
        ret, input_frame = self.cam.read()

        if not ret:
            print ('Error retriving video stream')
            return -9, None, None
        
        status, R, T, img = self._arucoTool.calculate_location(input_frame)
        
        if status == -9:
            if imshow:
                cv2.imshow(str(self.num), input_frame)
            return -9, None, None
        
        if imshow:
            cv2.imshow(str(self.num), img)
        
        return 0, R, T





    def trackLoop(self, target = {"lr":0, "fb":0, "ud":0}, bias = [0, 0]):

        status, R, T = self.get_location(True)
        
        if status == -9:
            self._tello.send_rc_control(0,0,0,0)
            print ("cant calculate moment")
            return [0,0]
        
        yaw = R["yaw"]
        lr = T["lr"]
        fb = T["fb"]
        ud = T["ud"]
        
        if not self.isLeader:
            # calc lr, fb before rotation
            lr_to_rotate = target["lr"] - lr + bias[0]
            fb_to_rotate = target["fb"] - fb + bias[1]
            
            # rotate lr, fb
            XY_vec = np.array([lr_to_rotate, fb_to_rotate])
            
            c, s = np.cos(-yaw), np.sin(-yaw)
            rotationMat = np.array(((c, -s), (s, c)))
            
            XY_vec_rotate = np.matmul(rotationMat, XY_vec)
            
            RClr = XY_vec_rotate[0]
            RCfb = XY_vec_rotate[1]
        
        # if leader ignore bias and rotation
        else:
            RClr = target["lr"] - lr
            RCfb = -(target["fb"] - fb)
            print (fb)

        # calc ud
        RCud = target["ud"] - ud
        # calc cw
        wanted_yaw = math.atan2(lr, fb) * (180/math.pi)
        RCcw = (wanted_yaw - yaw) * -1.7
        
        RClr = int(np.clip(RClr/5, -25, 25))
        RCfb = int(np.clip(RCfb/5, -25, 25))
        RCud = int(np.clip(RCud/5, -25, 25))
        RCcw = int(np.clip(RCcw, -50, 50))

        self._FrameCounter += 1

        self._tello.send_rc_control(RClr, RCfb, RCud, RCcw)
        
        if abs(RClr) < 7 and abs(RCfb) < 7 and abs(RCud) < 7:
            self.target_reached = True

        return [RClr, RCfb]