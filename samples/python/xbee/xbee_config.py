#!/usr/bin/python
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

def readline(ser):
	#A stupid function that I shouldn't need if pyserial wasn't hardcoded to only handle \n newlines...
	str = ""
	while 1:
		ch = ser.read()
		if(ch == '\r' or ch == ''):  
			break
		str += ch
	return str

def isOkay(ser):
	sleep(0.1)
	result = readline(ser)
	if result == "OK":
		return True
	return False

def getReplyFor(ser, atCommand):
	ser.write("AT" + atCommand + "\r")
	ser.flush()
	return readline(ser)

def enterCommandMode(ser):
	ser.write("+++")
	ser.flush()
	sleep(1)
	if (isOkay(ser)):
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

def read(ser):
	if (not(enterCommandMode(ser))):
		return False
	
	
	for i in range(0, len(AT_READ)):
		VALUES[i] = getReplyFor(ser, AT_READ[i])
		print(AT_READ[i] + ": " + VALUES[i])
		
	if (not(exitCommandMode(ser))):
		return False
	
	print("Read successful\n")
	
	#We could check for minimum firmware here... but I know that my modules are all fine.
	
	return True

def change(ser):
	#Choose a baud rate
	for i in range(0, len(BAUD_RATES)):
		print(" (" + str(i) + ") " + str(BAUD_RATES[i]))
	baudIndex = raw_input("Choose a baud rate [" + VALUES[2] + "]: ")
	if (baudIndex == ""):
		baudIndex = VALUES[2]
	bd = int(baudIndex)
	if (bd < 0 or bd >= len(BAUD_RATES)):
		print("Invalid input\n")
		return False

	#Choose a channel
#	channel = raw_input("Choose a Channel (B: XBee) (C: XBee Pro) [" + VALUES[3] + "]: ")
#	if (channel == ""):
#		channel = VALUES[3]
#	if (channel == 'B' or channel == 'C'):
#		ch = channel
#	else:
#		print("Invalid input\n")
#		return False

	#Choose the PAN ID
	panId = raw_input("Choose a PAN ID (0 - FFFE) [" + VALUES[4] + "]:")
	if (panId == ""):
		panId = VALUES[4]
	id = int("0x" + panId, 16)
	if (id < 0 or id > 0xfffe):
		print("Invalid input\n")
		return False
	
	#Is this a coordinator?
	coordinator = raw_input("Choose end device (0) or coordinator (1) [" + VALUES[5] + "]: ")
	if (coordinator == ""):
		coordinator = VALUES[5]
	ce = int(coordinator)
	if (ce < 0 or ce > 1):
		print("Invalid input\n")

	#TODO things we may want to think about
	# * MM -- probably want to use MM1
	# * RR -- ignored for MM1?
	# * RO -- probably want to use RO1
	
	if (ce == 0):
		my = 0
		dl = 0
	else:
		my = 1
		dl = 1
		
	atCommandString = "BD" + str(bd) + ",CE" + str(ce) + ",ID" + format(id, 'x') + ",MY" + str(my) + ",DL" + str(dl) + ",WR"
	
	confirm = raw_input(atCommandString + "\nOK to write? (y/n): ")
	if (confirm == "y" or confirm == "Y"):
		if (not(enterCommandMode(ser))):
			return False
		ser.write("ATRE\r")
		ser.flush()
		if(not(isOkay(ser))):
			print("Can't reset\n");
			return False;

		if getReplyFor(ser, atCommandString) != "OK":
			print("Cannot write configuration\n")
			return False
		
		if (not(exitCommandMode(ser))):
			return False
		
		print("Configuration update successful\n")
	else:
		print("Cancelled configuration update\n")
	

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
VALUES = [None] * len(AT_READ)

if (len(sys.argv) != 2):
	print("Usage: " + sys.argv[0] + " <port>")
	sys.exit(0)

for i in range(0, len(BAUD_RATES)):
	print(" (" + str(i) + ") " + str(BAUD_RATES[i]))

baudIndex = input("Choose a baud rate: ")

ser = serial.Serial(sys.argv[1], BAUD_RATES[baudIndex], timeout=2)
if (not(read(ser))):
	sys.exit(0)

changeConfig = raw_input("Change configuration? (y/n): ")
if (changeConfig == "Y" or changeConfig == "y"):
	change(ser)
