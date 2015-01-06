#!/usr/bin/env python2.6
#
# Reads from a serial port sending ascii data.
#
###################

import serial, sys
from time import sleep

ser = serial.Serial("/dev/tty.usbserial-FTE0U36U", 9600)

sleep(1);

while True:
	b1 = ser.read()
	b2 = ser.read()
	distance = (ord(b1) << 8) + ord(b2)
	sys.stdout.write("Distance (mm): " + str(distance) + "\n")
	#sys.stdout.write(b);

