#!/usr/bin/env python2.6
#
# Reads from a serial port sending ascii data.
#
###################

import serial, sys

ser = serial.Serial("/dev/tty.usbserial-FTE0U36U", 57600)

ser.write(chr(0x01))

while True:
        b = ser.read()
        sys.stdout.write(str(hex(ord(b))) + "\n")
        #sys.stdout.write(b);

