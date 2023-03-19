from djitellopy import Tello
from djitellopy import swarm
import cv2
import threading
from time import sleep

t = Tello("10.3.141.101")
t.connect()
t.takeoff()
t.move_up(50)
for i in range(30):
    t.send_rc_control(0,0,0,0)
    sleep(1)

