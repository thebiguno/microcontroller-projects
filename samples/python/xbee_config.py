#!/usr/bin/env python2.6
#
# XBee Configuration Program.
#
# Hardware Connections (XBee).  Remember that data + power is 3.3v
#  1 VCC
#  2 DOUT <---> RX (pin 5 on FTDI cable)
#  3 DIN  <---> TX (pin 4 on FTDI cable)
# 10 GND
#
###################

import serial, sys
from time import sleep


########## Helper functions here ##########

def isOkay(ser):
	sleep(0.1)
	return ser.readline() == "OK"

def enterCommandMode(ser):
	ser.write("+++")
	ser.flush()
	sleep(1)
	if (isOkay()):
		print("Entered command mode")
		return True
	else:
		print("Cannot enter command mode")
		return False
		
def exitCommandMode(ser):
	ser.write("ATCN\r")
	ser.flush()
	if (isOkay):
		print("Exited command mode")
		return True
	else:
		print("Cannot exit command mode")
		return False

########## Main program starts here ##########

MINIMUM_FIRMWARE = 0x10A5
BAUD_RATES = [1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200]
AT_READ = [
		"SH", # serial number high 
		"SL", # serial number low 
		"BD", # baud rate
		"CH", # channel
		"ID", # pan id
		"CE", # coordinator enable
		"RR", # retries
		"RO", # packetization timeout
		"MY", # my id 
		"DL", # destination address low 
		"VR", # firmware version
]

if (len(sys.argv) != 2):
	print("Usage: " + sys.argv[0] + " <port>")
	sys.exit(0)

for i in range(0, len(BAUD_RATES)):
	print(" (" + str(i) + ") " + str(BAUD_RATES[i]))

baudIndex = input("Choose a baud rate: ")

ser = serial.Serial(sys.argv[1], BAUD_RATES[baudIndex])


if (not(enterCommandMode())):
	sys.exit(-1)
	



while True:
        b = ser.read()
        sys.stdout.write(str(hex(ord(b))) + " (" + b + ")\n")
        #sys.stdout.write(b);

