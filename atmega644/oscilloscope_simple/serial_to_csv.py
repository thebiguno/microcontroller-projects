#!/opt/local/bin/python2.6
#
# Reads from a serial port sending binary data, and writes to CSV format.
#
###################

import serial

ser = serial.Serial("/dev/tty.usbserial-FTE0U36U", 9600)

last = 0x0

while True:
	time = ord(ser.read())
	time = (time * 256) + ord(ser.read())
	time = (time * 256) + ord(ser.read())
	time = (time * 256) + ord(ser.read())

	value = ord(ser.read())
	
	print(
		str(time - 1) + "," + 
		str((last >> 3) & 0x1) + "," + 
		str((last >> 2) & 0x1) + "," + 
		str((last >> 1) & 0x1) + "," + 
		str((last >> 0) & 0x1))

	last = value;
	
	print(
		str(time) + "," + 
		str((value >> 3) & 0x1) + "," + 
		str((value >> 2) & 0x1) + "," + 
		str((value >> 1) & 0x1) + "," + 
		str((value >> 0) & 0x1))

