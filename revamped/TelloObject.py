import cv2
from djitellopy import Tello
import VCS
from ArucoTools import ArucoTools
import math

class TelloObject:

    def __init__(self, address:str, vport:int, arucoTool:ArucoTools, num, ABS_POS, startCam = True):
        # connection vals
        self._vport = vport
        self._address = address
        
        self.num = num
        
        self.target_reached = False
        
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
    
        cw = int(wanted_yaw - yaw) * -1
        
        self._tello.send_rc_control(0,0,0,cw)
        
        if abs(cw) < 8:
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





    def trackLoop(self):

        ret, input_frame = self.cam.read()

        if not ret:
            print ('Error retriving video stream')
            return lr, fb, ud
    
        status, lr, fb, ud, cw, img = self._arucoTool.arucofunc(input_frame, self._distance, self._angle, self._hight)
        
        lr = int(lr)
        fb = int(fb)
        ud = int(ud)
        cw = int(cw)

        # save img to path
        filename = 'video/' + str(self.num) + '/' + str(self._FrameCounter) + '.jpg'
        cv2.imwrite(filename, img)

        self._FrameCounter += 1

        self._tello.send_rc_control(lr, fb, ud, cw)
        
        if status == 5:
            self.target_reached = True

        img = cv2.resize(img, (720, 480))
        cv2.imshow(str(self.num), img)