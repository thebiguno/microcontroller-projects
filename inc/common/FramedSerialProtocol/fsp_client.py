#!/usr/bin/python
#
# Python implementation of the Framed Serial Protocol.  Reads the raw values,
# and interprets them using the specified logic.
#
###################

import serial, sys
from time import sleep

# Argument is serial port
ser = serial.Serial(sys.argv[1], sys.argv[2])

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
	c = ser.read()
	b = ord(c)
	ser.write(c)
	
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
		sys.stdout.write("Start of Frame: 0x7e\n");
		continue
	elif (pos == 1):
		length = b
		sys.stdout.write("Length: " + hex(b) + "\n");
		pos = pos + 1
		continue
	elif (pos == 2):
		cmd = b
		sys.stdout.write("Command: " + hex(b) + "\n");
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
				sys.stdout.write("Finished Command " + hex(cmd) + "\n")
				
			else:
				err = True;
				sys.stdout.write("Invalid checksum\n")
			pos = 0;
			chk = 0;
		else:
			buf[pos - 3] = b
			sys.stdout.write("Data: " + hex(b) + "\n");
			pos = pos + 1
	
