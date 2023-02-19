from djitellopy import Tello
from djitellopy import TelloSwarm
import cv2

#tello0 = Tello("10.3.141.177")
tello1 = Tello("10.3.141.124")

sworm = TelloSwarm()

#tello0.connect()
tello1.connect()

#print ("Battery level : ", end="")
#print (tello0.get_battery())

print ("Battery level : ", end="")
print (tello1.get_battery())

#tello0.streamon()
tello1.streamon()


#frame_read0 = tello0.get_frame_read()
frame_read1 = tello1.get_frame_read()

while cv2.waitKey(1) != ord("q"):

    #img0 = frame_read0.frame
    #cv2.imshow("drone0", img0)

    img1 = frame_read1.frame
    cv2.imshow("drone1", img1)

#tello0.streamoff()
tello1.streamoff()

#tello = Tello()

#tello.connect()

#tello.connect_to_wifi("rpi-hotspot", "rpiServer-1qaz")

#exit()
#tello.takeoff()


#tello.land() 