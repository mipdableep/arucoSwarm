from djitellopy import Tello

'''
Connect to tello dron, and set to connect to our network (rpi-hotspot)
'''

tello = Tello()

tello.connect()

tello.connect_to_wifi("rpi-hotspot", "rpiServer-1qaz")

exit(1)