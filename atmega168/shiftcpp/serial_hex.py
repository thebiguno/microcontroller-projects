#!/opt/local/bin/python2.6
#
# Reads from a serial port sending binary data.
#
###################

import serial

ser = serial.Serial("/dev/tty.usbserial-FTE0U36U", 9600)

while True:
	b = ord(ser.read())
	
	print(hex(b));

