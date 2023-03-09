from djitellopy import Tello
import cv2
import numpy as np


def rect (angle, radius):
    return np.array([radius * np.cos(angle), radius * np.sin(angle)])

def rotate (tello : Tello, angle, radius, arc, speed):

    angle, arc = np.deg2rad(angle), np.deg2rad(arc)

    # Center of the circle of rotation
    pivot = rect (angle, radius)

    # First point
    p1 = pivot + rect (-np.pi + arc/2 + angle, radius)

    p2 = pivot + rect (-np.pi + arc + angle, radius)

    tello.curve_xyz_speed(int(p1[0]), int(p1[1]), 0, int(p2[0]), int(p2[1]), 0, speed)


tello1 = Tello("10.3.141.169")
tello1.connect()

tello1.takeoff()

rotate (tello1, -45,  55, 180, 60)
rotate (tello1, -45, -55, 180, 60)
rotate (tello1, -45,  55, 180, 60)
rotate (tello1, -45, -55, 180, 60)

'''
#                       x1    y1    z1    x2    y2    z2   spd
tello1.curve_xyz_speed( -50,  -50,    0,    0, -100,    0,  20)
tello1.curve_xyz_speed(  50,   50,    0,    0,  100,    0,  20)
'''

tello1.land()
