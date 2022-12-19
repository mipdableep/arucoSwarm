# External module imports
import RPi.GPIO as GPIO
import time

# Pin Definitons:
ledPin = 40

# Pin Setup:
GPIO.setmode(GPIO.BOARD)
GPIO.setup(ledPin, GPIO.OUT) # LED pin set as output

cleanUp = GPIO.cleanup()

counter = 0
try:
    GPIO.output(ledPin, GPIO.HIGH)
    time.sleep(15)
except (KeyboardInterrupt):
    GPIO.cleanup()
    exit(0)

GPIO.cleanup()

