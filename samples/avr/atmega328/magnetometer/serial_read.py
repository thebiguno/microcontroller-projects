#!/usr/bin/python
#
# Reads from a serial port sending ascii data.
#
###################

import serial, sys
from time import sleep

ser = serial.Serial("/dev/ttyUSB0", 9600)

sleep(1);

while True:
	b = ser.read()
	sys.stdout.write(b)

