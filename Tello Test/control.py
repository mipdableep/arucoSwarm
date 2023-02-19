from djitellopy import Tello
import cv2

tello = Tello()
tello.connect()

print ("Battery level : ", end="")
print (tello.get_battery())

tello.streamon()
frame_read = tello.get_frame_read()

while True:

    img = frame_read.frame
    cv2.imshow("drone", img)

    lr = 0
    ud = 0
    fb = 0
    rc = 0

    key = cv2.waitKey(1) & 0xff
    if key == 27: # ESC
        break
    elif key == ord('w'):
        fb = 50
    elif key == ord('s'):
        fb = -50
    elif key == ord('a'):
        lr = -50
    elif key == ord('d'):
        lr = 50
    elif key == ord('e'):
        rc = 50
    elif key == ord('q'):
        rc = -50
    elif key == ord('r'):
        ud = 50
    elif key == ord('f'):
        ud = -50
    elif key == ord('z'):
        tello.takeoff()
    elif key == ord('x'):
        tello.land()

    tello.send_rc_control(lr, fb, ud, rc)

tello.land()