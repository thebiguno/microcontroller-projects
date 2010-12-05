#!/opt/local/bin/python2.6
#
# Reads from a serial port sending ascii data.
#
###################

import serial

ser = serial.Serial("/dev/tty.usbserial-FTE0U36U", 57600)

ser.write("Hello World!  The quick brown fox jumped over the lazy dog.")

while True:
        b = ser.read()
        print(str(hex(ord(b))) + " (" + b + ")");

