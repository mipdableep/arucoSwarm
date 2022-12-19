# External module imports
import RPi.GPIO as GPIO
import time

# Pin Definitons:
ledPin = 12 # Broadcom pin 23 (P1 pin 16)

# Pin Setup:
GPIO.setmode(GPIO.BOARD)
GPIO.setup(ledPin, GPIO.OUT) # LED pin set as output

counter = 0
try:
    while counter < 20:
        GPIO.output(ledPin, GPIO.HIGH)
        time.sleep(0.8)
        GPIO.output(ledPin, GPIO.LOW)
        time.sleep(0.8)
        counter += 1
except (KeyboardInterrupt):
    GPIO.cleanup()
    exit(0)

