#!/opt/local/bin/python2.6
#
# Reads from a serial port sending binary data, and writes to CSV format.
#
###################

import serial

ser = serial.Serial("/dev/tty.usbserial-FTE0U36U", 76800)

last = 0x0

while True:
	time = ord(ser.read())
	time = (time << 8) + ord(ser.read())
	time = (time << 8) + ord(ser.read())
	time = (time << 8) + ord(ser.read())

	value = time & 0x1
	time = time & ~0x1
	
	print(
		str(time - 1) + "," + 
		str(last))

	last = value
	
	print(
		str(time) + "," + 
		str(value))

