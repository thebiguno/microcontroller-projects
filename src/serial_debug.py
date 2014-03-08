#!/usr/bin/python
#
# Reads from a serial port sending ascii data.
#
###################

import serial
import sys

ser = serial.Serial(sys.argv[1], 76800)

while True:
	b = ser.read()
	
	sys.stdout.write(str(hex(ord(b))) + "\n")

