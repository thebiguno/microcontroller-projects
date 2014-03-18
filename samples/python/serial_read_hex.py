#!/usr/bin/env python2.6
#
# Reads from a serial port sending ascii data.
#
###################

import serial, sys
from time import sleep

ser = serial.Serial("/dev/tty.usbserial-FTE0U36U", sys.argv[1])

sleep(1);

while True:
        b = ser.read()
        sys.stdout.write(str(hex(ord(b))) + " (" + b + ")\n")
        #sys.stdout.write(b);

