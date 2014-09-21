#!/usr/bin/env python2.6
#
# Reads from a serial port sending ascii data.
#
###################

import serial, sys
from time import sleep

ser = serial.Serial("/dev/tty.Stubby-DevB", 38400)

ser.write("\x93")		#Start (to begin)

#while True:
ser.write("\x0F")		#Left X to neutral
ser.write("\x2F")		#Left Y to neutral
ser.write("\x4F")		#Right X to neutral
ser.write("\x6F")		#Right Y to neutral
sleep(0.01)

#Movement
ser.write("\x20")		#Forward
ser.write("\x10")		#Left X to neutral
sleep(1)
ser.write("\x3F")		#Backward
ser.write("\x10")		#Left X to neutral
sleep(1)

ser.write("\x0F")		#Left X to neutral
ser.write("\x2F")		#Left Y to neutral
sleep(1)

ser.write("\x93")		#Start (to end)

#b = ser.read()
#sys.stdout.write(str(hex(ord(b))) + " (" + b + ")\n")
#sys.stdout.write(b);

