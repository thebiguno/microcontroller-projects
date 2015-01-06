#!/usr/bin/python
#
# Python implementation of the Universal Controller framing protocol.  Reads the raw values,
# and interprets them using the specified logic.
#
###################

import serial, sys
from time import sleep

# Argument is serial port
ser = serial.Serial(sys.argv[1], 38400)

sleep(1);

START = 0x7e
ESCAPE = 0x7d
MAX_SIZE = 40

err = False
esc = False
pos = 0
length = 0
cmd = 0
chk = 0x00

buf = {}

while True:
	b = ord(ser.read())
	
	if (err and b == START):
		# recover from error condition
		sys.stdout.write("Recover from error condition\n")
		err = False
		pos = 0
	elif (err):
		continue

	if (pos > 0 and b == START):
		# unexpected start of frame
		sys.stdout.write("Unexpected start of frame\n")
		err = True
		continue

	if (pos > 0 and b == ESCAPE):
		# unescape next byte
		esc = True
		continue
	
	if (esc):
		# unescape current byte
		b = 0x20 ^ b
		esc = False

	if (pos > 1):
		chk = (chk + b) & 0xFF
	
	if (pos == 0):
		pos = pos + 1
		continue
	elif (pos == 1):
		length = b
		pos = pos + 1
		continue
	elif (pos == 2):
		cmd = b
		pos = pos + 1
		continue
	else:
		if (pos > MAX_SIZE):
			# this probably can't happen
			sys.stdout.write("Position > MAX_SIZE\n")
			continue

		if (pos == (length + 2)):
			if (chk == 0xff):
				#Finished the message; decode it
				if (cmd == 0x00):
					sys.stdout.write("Control ID: " + chr(buf[0]) + "\n")
				elif (cmd == 0x10 or cmd == 0x11):
					sys.stdout.write("Button ")
					button = buf[0]
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

					if (cmd == 0x10):
						sys.stdout.write("Pressed\n")
					else:
						sys.stdout.write("Released\n")
				elif (cmd == 0x12):
					sys.stdout.write("No buttons pressed\n")
				elif (cmd == 0x13): #Analog stick value
					sys.stdout.write("Left X = " + format(buf[0], '#02x') + "\n")
					sys.stdout.write("Left Y = " + format(buf[1], '#02x') + "\n")
					sys.stdout.write("Right X = " + format(buf[2], '#02x') + "\n")
					sys.stdout.write("Right Y = " + format(buf[3], '#02x') + "\n")
				
			else:
				err = True;
				sys.stdout.write("Invalid checksum\n")
			pos = 0;
			chk = 0;
		else:
			buf[pos - 3] = b
			pos = pos + 1
