#!/opt/local/bin/python2.6
#
# Reads from a serial port sending ascii data.
#
###################

import serial

ser = serial.Serial("/dev/tty.usbserial-FTE0U36U", 2400)

ser.write("ABCDEFGHIJLKMNOPQRSTUVWXY")

