
DEFAULT_CLIP = 40
SAFTY_CLIP = 30
MAX_CLIP = 100

'''
Fix the value to be competible with the tello SDK.
Tello accepts only integer values between -100 to 100
'''
def fixValue (x : float, clip = DEFAULT_CLIP):
    # Floor to 100 if needed
    if x > clip:
        x = clip

    # Ceil to -100 when neede
    if x < -clip:
        x = -clip
    
    # Round to integer
    return int(x)


def extractVector(vec):
    x = vec[0,0]
    y = vec[1,0]
    z = vec[2,0]

    return x, y, z