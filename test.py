import numpy as np
import pygame
import math

#tal
# alpha is the angle between the origin and drone (given as yaw/Zr)
# sin or cosin of alpha is the vector in:

# base = np.array([sin(alpah), y, z]
#                 [cosin(alpha), y, z]
#                 [0, 0, z])

#-----------------------------tomer
0 # # #
# u # r
# # d #
# # # #

#r - d  this is a point - meaning (x,y)
#u - d  also point, (a,b)
#the point is (p,q)
"""
base = [x,a,p]
       [y,b,q]
       [0,0,1]
"""
#base ^-1 * any vector ([x, y, 1]) will be the coardinits

dev = 12

pygame.init()
WIDTH, HIGHT = 800, 800
WIN = pygame.display.set_mode((WIDTH, HIGHT))
pygame.display.set_caption("planet sim")

WHITE = (255,255,255)
BLACK = (0,0,0)
RED = (255,0,0)
GREEN = (0,255,0)
DARKGRAY = (20,20,20)
LIGHTGRAY = (150,150,150)
BLUE = (0,0,255)
PRPL = (255,0,255)



theta = np.radians(-90)
c, s = np.cos(theta), np.sin(theta)
R = np.array(((c, -s), (s, c)))
print ("R = ", R)

v = np.array([3,-2])
print ("v = ", v)

T = np.array([2,-3])
print ("T = ", T)

Rv = np.matmul(R, v)
print ("Rv = ", Rv)

TRv = Rv - T
print ("TRv = ", TRv)

# Plot w
origin = np.array([0,0])

def drawPoint(color, loc, radius = 7):
    pygame.draw.circle(WIN, color, (loc[0]*WIDTH/dev + WIDTH/2, -loc[1]*HIGHT/dev + HIGHT/2), radius)
    

def main():
    run = True
    clock = pygame.time.Clock()
    x = 0
    y = 0
    
    while run:
        clock.tick(60)
        WIN.fill(WHITE)

        pygame.draw.line(WIN, BLACK, (HIGHT/2, 0), (HIGHT/2, WIDTH), 3)
        pygame.draw.line(WIN, BLACK, (0, WIDTH/2), (HIGHT, WIDTH/2), 3)
        for i in [x+1 for x in range(dev)]:
            pygame.draw.line(WIN, LIGHTGRAY, (0, i*(WIDTH/dev)), (HIGHT, i*(WIDTH/dev)))
            pygame.draw.line(WIN, LIGHTGRAY, (i*(WIDTH/dev), 0), (i*(WIDTH/dev), HIGHT))

        # drawPoint(BLUE, v)
        # drawPoint(BLACK, T)

        pygame.display.update()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit
                run = False
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_q:
                    pygame.quit
                    run = False

                # if event.key == pygame.K_RIGHT:
                #     x += 1
                #     print(x, "  ", y)
                # if event.key == pygame.K_DOWN:
                #     y -= 1
                #     print(x, "  ", y)
                # if event.key == pygame.K_LEFT:
                #     x -= 1
                #     print(x, "  ", y)
                # if event.key == pygame.K_UP:
                #     y += 1
                #     print(x, "  ", y)

if __name__ == '__main__':
    main()