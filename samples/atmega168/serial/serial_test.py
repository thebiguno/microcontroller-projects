#!/usr/bin/env python2.6
#
# Reads from a serial port sending ascii data.
#
###################

import serial, sys
from time import sleep

ser = serial.Serial("/dev/tty.usbserial-FTE0U36U", sys.argv[1])

#Give time to init AVR before bombarding it with data
sleep(1);

i = 0
while True:
        b = ser.read()
	if (ord(b) != i):
		print("Bitstream corruption!  Expected " + hex(i) + ", recieved " + hex(ord(b)))
		i = -1
	else:
       		print(str(hex(ord(b))))
	i = i + 1
	if (i > 0x3F):
		i = 0

