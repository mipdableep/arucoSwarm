# External module imports
import RPi.GPIO as GPIO
import time

# Pin Definitons:
ledPin = 12 # Broadcom pin 23 (P1 pin 16)
switchPin = 21 # Broadcom pin 17 (P1 pin 11)


# Pin Setup:
GPIO.setmode(GPIO.BOARD)
GPIO.setup(ledPin, GPIO.OUT) # LED pin set as output
GPIO.setup(switchPin, GPIO.IN, pull_up_down=GPIO.PUD_UP) # Button pin set as input w/ pull-up

try:
    while 1:
        if not GPIO.input(switchPin):
            GPIO.output(ledPin, GPIO.HIGH)
        else:
            GPIO.output(ledPin, GPIO.LOW)
        time.sleep(0.1)
except(KeyboardInterrupt):
    GPIO.cleanup()