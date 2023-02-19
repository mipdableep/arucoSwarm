# Imports
import numpy as np
from ctypes import *

lib = CDLL("/home/daniel/Documents/Image Test/funcs_lib.so")

toGray_c = lib.toGray
toGray_c.argtypes = [np.ctypeslib.ndpointer(c_uint8, flags="C_CONTIGUOUS"), c_uint, c_uint]

balanceColors_c = lib.balanceColors
balanceColors_c.argtypes = [np.ctypeslib.ndpointer(c_uint8, flags="C_CONTIGUOUS"), c_uint, c_uint, c_float, c_float, c_float]

findPic_c = lib.findPic
findPic_c.argtypes = [np.ctypeslib.ndpointer(c_uint8, flags="C_CONTIGUOUS"), c_uint, c_uint]

def toGray(mat : np.ndarray):

    # The array must be 3-dimention array (2D with BGR)
    assert (np.ndim(mat) == 3)

    n = np.size(mat, axis=0)
    m = np.size(mat, axis=1)
    z = np.size(mat, axis=2)

    # Third dimantion must be in size 3, for BGR
    assert(z == 3)

    mat = np.copy(mat)

    toGray_c.restype = np.ctypeslib.ndpointer(dtype=c_uint8, shape=(n, m))

    return toGray_c(mat, n, m)


def balanceColors(mat : np.ndarray, blue : float, green : float, red : float):

    # The array must be 3-dimention array (2D with BGR)
    assert (np.ndim(mat) == 3)

    n = np.size(mat, axis=0)
    m = np.size(mat, axis=1)
    z = np.size(mat, axis=2)

    # Third dimantion must be in size 3, for BGR
    assert(z == 3)

    mat = np.copy(mat)

    balanceColors_c.restype = np.ctypeslib.ndpointer(dtype=c_uint8, shape=(n, m, 3))

    return balanceColors_c(mat, n, m, blue, green, red)


def findPic(mat : np.ndarray):

    # The array must be 2-dimention array (gray scale image)
    assert (np.ndim(mat) == 2)

    n = np.size(mat, axis=0)
    m = np.size(mat, axis=1)

    print (str(n) + " : " + str(m))

    mat = np.copy(mat)

    findPic_c.restype = np.ctypeslib.ndpointer(dtype=c_uint8, shape=(n - 10, m - 10))

    return findPic_c(mat, n, m)