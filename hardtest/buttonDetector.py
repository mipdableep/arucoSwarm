# External module imports
import subprocess
import RPi.GPIO as GPIO
from time import sleep
import sys

# Pin Definitons:
ledPin = 12 # Broadcom pin 23 (P1 pin 16)
switchPin = 21 # Broadcom pin 17 (P1 pin 11)

# Pin Setup:
GPIO.setmode(GPIO.BOARD)
GPIO.setup(ledPin, GPIO.OUT) # LED pin set as output
GPIO.setup(switchPin, GPIO.IN, pull_up_down=GPIO.PUD_UP) # Button pin set as input w/ pull-up

counter = 0

while (counter < 5):
    GPIO.output(ledPin, GPIO.HIGH)
    sleep(0.5)
    GPIO.output(ledPin, GPIO.LOW)
    sleep(0.5)
    counter += 1

counter = 0

if not GPIO.input(switchPin):
    GPIO.cleanup()
    print("pressed")
    subprocess.run(["python3", "/home/pi/arucoSwarm/hardtest/ledPress.py"])
else:
    GPIO.cleanup()
    print("not pressed")
    subprocess.run(["/home/pi/arucoSwarm/hardtest/test.sh"], shell=True)
