#!/usr/bin/env python2.6
#
# Reads from a serial port sending ascii data.
#
###################

import serial, array
from time import sleep

ser = serial.Serial("/dev/tty.usbserial-FTE0U36U", 57600)

i = 1

while True:
	sleep(0.1)
	s = array.array('B', [i]).tostring()
	print(i)
	ser.write(s)
	i = i + 1;
	if (i > 0xFF):
		i = 1

