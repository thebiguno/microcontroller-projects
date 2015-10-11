#!/usr/bin/python
#
# Reads from a serial port sending ascii data.
#
###################

import serial, sys
from time import sleep

ser = serial.Serial(sys.argv[1], sys.argv[2])

sleep(1);

while True:
        b = ser.read()
        sys.stdout.write(str(hex(ord(b))) + "\n")
        #sys.stdout.write(b);
        message = [0x7e, 0x01, 0x12, 0xed]
        ser.write(''.join(chr(b) for b in message))
        ser.flush()
