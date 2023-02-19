from djitellopy import Tello
import cv2

tello1 = Tello("10.3.141.159")
tello1.connect()

tello1.set_network_ports(8890, 11113)
tello1.streamon()

tello1.takeoff()

cam1 = cv2.VideoCapture("udp://10.3.141.159:11113")

while cv2.waitKey(1) != ord("q"):

    ret, frame1 = cam1.read()
    frame1 = cv2.resize(frame1, (300, 300))
    cv2.imshow("video1", frame1)

    
