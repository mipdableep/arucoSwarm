import cv2
import numpy as np


def np2yaml(calibPath):
    with open(calibPath, 'rb') as f:
        camMatrix = np.load(f)
        camDist = np.load(f)
    
    # Save calibration data to YAML file
    fs = cv2.FileStorage('calib.yaml', cv2.FILE_STORAGE_WRITE)
    fs.write('camera_matrix', camMatrix)
    fs.write('distortion_coefficients', camDist)
    fs.release()