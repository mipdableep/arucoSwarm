import cv2
from djitellopy import Tello
import VCS
from ArucoTools import ArucoTools

class TelloObject:

    def __init__(self, address:str, vport:int, arucoTool:ArucoTools, num, ABS_POS = (0,0,0)):
        # connection vals
        self._vport = vport
        self._address = address
        
        self.num = num
        
        self.target_reached = False
        
        # location vals
        self._ABS_POS = ABS_POS
        
        
        self._retErr = (-1,-1,-1)
        
        self._arucoTool = arucoTool
        self._FrameCounter = 0
        
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


    def get_location(self):
        
        ret, input_frame = self.cam.read()

        if not ret:
            print ('Error retriving video stream')
            return self._retErr
        
        status, location, img = self._arucoTool.arucofunc(input_frame, self._distance, self._angle, self._hight)





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