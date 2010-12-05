#!/opt/local/bin/python2.6
#
# Reads from a serial port sending ascii data.
#
###################

import serial, sys

ser = serial.Serial("/dev/tty.usbserial-FTE0U36U", 2400)

ser.write("Hello World!\nThe quick brown fox jumped over the lazy dog.\n")

while True:
        b = ser.read()
        #sys.stdout.write(str(hex(ord(b))) + " (" + b + ")");
        sys.stdout.write(b);

