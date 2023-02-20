import djitellopy
from time import sleep
import threading

def move_frwd(tello):
    tello.move_forward(300)

T1 = djitellopy.Tello("10.3.141.67")
T2 = djitellopy.Tello("10.3.141.226")


T1.connect()
T2.connect()

sleep(2)

T1.takeoff()
T2.takeoff()

# T1.send_rc_control(0, 0, 0, 0)
# T2.send_rc_control(0, 0, 0, 0)

# sleep(2)

threading.Thread(target=move_frwd, args=(T1))
threading.Thread(target=move_frwd, args=(T2))

