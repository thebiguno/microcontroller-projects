#!/usr/bin/python
#
# Reads from a serial port sending ascii data.
#
###################

import serial, sys
from time import sleep

# First argument is serial port, second is baud rate.
ser = serial.Serial(sys.argv[1], sys.argv[2])

sleep(1);

while True:
	b = ord(ser.read())

	if (b & 0x80) == 0x00:	#Analog stick value
		sys.stdout.write("Stick ")
		if (b & 0x40) == 0x00:
			sys.stdout.write("Left ")
		else:
			sys.stdout.write("Right ")
		if (b & 0x20) == 0x00:
			sys.stdout.write("X = ")
		else:
			sys.stdout.write("Y = ")
		sys.stdout.write(format(b & 0x1F, '#04x'))
	elif (b & 0x40) == 0x00:	#Normal button press (i.e. not 'No buttons pressed' event)
		sys.stdout.write("Button ")
		button = b & 0x0F
		if button == 0x00:
			sys.stdout.write("Select ")
		elif button == 0x01:
			sys.stdout.write("Left 3 ")
		elif button == 0x02:
			sys.stdout.write("Right 3 ")
		elif button == 0x03:
			sys.stdout.write("Start ")
		elif button == 0x04:
			sys.stdout.write("Pad Up ")
		elif button == 0x05:
			sys.stdout.write("Pad Right ")
		elif button == 0x06:
			sys.stdout.write("Pad Down ")
		elif button == 0x07:
			sys.stdout.write("Pad Left ")
		elif button == 0x08:
			sys.stdout.write("Left 2 ")
		elif button == 0x09:
			sys.stdout.write("Right 2 ")
		elif button == 0x0A:
			sys.stdout.write("Left 1 ")
		elif button == 0x0B:
			sys.stdout.write("Right 1 ")
		elif button == 0x0C:
			sys.stdout.write("Triangle ")
		elif button == 0x0D:
			sys.stdout.write("Circle ")
		elif button == 0x0E:
			sys.stdout.write("Cross ")
		elif button == 0x0F:
			sys.stdout.write("Square ")

		sys.stdout.write("(" + hex(button) + ") ")

		if (b & 0x10) == 0x10:
			sys.stdout.write("Pressed")
		else:
			sys.stdout.write("Released")
	elif b == 0xC0: #No buttons pressed
		sys.stdout.write("No buttons pressed")
	else:
		sys.stdout.write("Unknown command")
	sys.stdout.write(" (" + hex(b) + ")\n");

	ser.write(chr(0x41))	#Enable analog sticks
	ser.write(chr(0xFF))	#Analog repeat of about 32ms


