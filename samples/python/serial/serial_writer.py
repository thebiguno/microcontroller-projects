#!/usr/bin/env python2.6
#
# Sends 'X' every 500ms
#
###################

import serial, sys
from time import sleep

ser = serial.Serial(sys.argv[1], sys.argv[2])

#Give time to init AVR before bombarding it with data
sleep(1);

while True:
		ser.write("X")
		sleep(0.5)
