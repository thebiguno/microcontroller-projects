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

while True:
	i = 0
	j = 0
	while (i <= 0xF):
		while (j <= 0xF):
			ser.write(hex(i << 4 | j))
			j = j + 1
		sleep(0.5)
		i = i + 1
		j = 0
