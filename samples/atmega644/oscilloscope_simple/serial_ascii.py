#!/opt/local/bin/python2.6
#
# Reads from a serial port sending ascii data.
#
###################

import serial
import sys

ser = serial.Serial("/dev/tty.usbserial-FTE0U36U", 9600)

while True:
	b = ser.read()
	
	sys.stdout.write(b);

