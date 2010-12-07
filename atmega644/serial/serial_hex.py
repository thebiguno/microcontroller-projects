#!/usr/bin/python
#
# Reads from a serial port sending ascii data.
#
###################

import serial

ser = serial.Serial("/dev/tty.usbserial-A8007UF6", 9600)

ser.write("Hello World!")

while True:
        b = ser.read()
        print(str(hex(ord(b))) + " (" + b + ")");

