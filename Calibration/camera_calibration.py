import numpy as np
import cv2 as cv
import glob

# Chess board crosses
cross = (5, 7)

# termination criteria
criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 30, 0.001)

# prepare object points
objp = np.zeros((np.prod(cross), 3), np.float32)
objp[:,:2] = 6 * np.mgrid[0:cross[0], 0:cross[1]].T.reshape(-1, 2)

# Arrays to store object points and image points from all the images.
objpoints = [] # 3d point in real world space
imgpoints = [] # 2d points in image plane.

images = glob.glob('/home/daniel/Documents/arucoSwarm/Camera Calibration/Images5/*.jpg')

for fname in images:

    print(fname)

    img = cv.imread(fname)
    gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)

    # Find the chess board corners
    ret, corners = cv.findChessboardCorners(gray, cross, None)

    # If found, add object points, image points (after refining them)
    if ret:
        objpoints.append(objp)
        corners2 = cv.cornerSubPix(gray,corners, (11,11), (-1,-1), criteria)
        imgpoints.append(corners2)
        # Draw and display the corners
        #cv.drawChessboardCorners(img, cross, corners2, ret)

        #cv.imshow('img', img[0:-1:2,0:-1:2])
        #cv.waitKey(300)

cv.destroyAllWindows()

ret, mtx, dist, rvecs, tvecs = cv.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)

print (mtx)
print (dist)