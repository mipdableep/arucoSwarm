from djitellopy import Tello
from djitellopy import swarm
import cv2
import threading
from time import sleep

'''
tello = Tello()

tello.connect()
tello.connect_to_wifi("rpi-hotspot", "rpiServer-1qaz")

exit()
'''

tello0 = Tello("10.3.141.226")
tello1 = Tello("10.3.141.159")

tello0.connect()
tello1.connect()

tello0.set_network_ports(8890, 11112)
tello1.set_network_ports(8890, 11113)

tello0.streamon()
tello1.streamon()

cam0 = cv2.VideoCapture("udp://10.3.141.226:11112")
cam1 = cv2.VideoCapture("udp://10.3.141.159:11113")

while cv2.waitKey(1) != ord("q"):

    ret, frame0 = cam0.read()
    frame0 = cv2.resize(frame0, (300, 300))
    cv2.imshow("video0", frame0)

    ret, frame1 = cam1.read()
    frame1 = cv2.resize(frame1, (300, 300))
    cv2.imshow("video1", frame1)


exit()

#tello0.streamon()
#tello1.streamon()


#frame_read0 = tello0.get_frame_read()
#frame_read1 = tello1.get_frame_read()

while cv2.waitKey(1) != ord("q"):

    img0 = frame_read0.frame
    cv2.imshow("drone0", img0)

    #img1 = frame_read1.frame
    #cv2.imshow("drone1", img1)

tello0.streamoff()
#tello1.streamoff()

#tello = Tello()

#tello.connect()

#tello.connect_to_wifi("rpi-hotspot", "rpiServer-1qaz")

#exit()
#tello.takeoff()


#tello.land()