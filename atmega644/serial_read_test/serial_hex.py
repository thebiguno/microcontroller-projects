#!/usr/bin/env python2.6
#
# Reads from a serial port sending ascii data.
#
###################

import serial, sys

ser = serial.Serial("/dev/tty.usbserial-FTE0U36U", 57600)

ser.write("Hello World!\nThe quick brown fox jumped over the lazy dog.\nThis is a long test... how long does it need to be?  I don't know.  Why do you ask?\n")

while True:
        b = ser.read()
        #sys.stdout.write(str(hex(ord(b))) + " (" + b + ")\n")
        sys.stdout.write(b);

