#!/usr/bin/env python2.6
#
# Reads from a serial port sending ascii data.
#
###################

import serial, sys
from time import sleep

ser = serial.Serial(sys.argv[1], sys.argv[2])

#Give time to init AVR before bombarding it with data
sleep(1);

while True:
		print("Foo")
		ser.write("\x7e")
		ser.write("\x11")
		ser.write("\x05")
		ser.write("\x48")
		ser.write("\x65")
		ser.write("\x6c")
		ser.write("\x6c")
		ser.write("\x6f")
		ser.write("\x20")
		ser.write("\x57")
		ser.write("\x6f")
		ser.write("\x72")
		ser.write("\x6c")
		ser.write("\x64")
		ser.write("\x21")
		ser.write("\x21")
		ser.write("\x21")
		ser.write("\x21")
		ser.write("\x21")
		ser.write("\x39")
		sleep(0.5)
		ser.write("\x7e")
		ser.write("\x11")
		ser.write("\x05")
		ser.write("\x47")
		ser.write("\x6f")
		ser.write("\x6f")
		ser.write("\x64")
		ser.write("\x62")
		ser.write("\x79")
		ser.write("\x65")
		ser.write("\x20")
		ser.write("\x57")
		ser.write("\x6f")
		ser.write("\x72")
		ser.write("\x6c")
		ser.write("\x64")
		ser.write("\x21")
		ser.write("\x21")
		ser.write("\x21")
		ser.write("\xa6")
		sleep(0.5)
