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

MINIMUM_FIRMWARE = 0x10A5

def writeGeneric(atCommand):
	if (atCommand["newValue"] == None):
		return None
	return atCommand["at"] + str(atCommand["newValue"])

def writePan(atCommand):
	if (atCommand["newValue"] == None):
		return None
	return atCommand["at"] + format(atCommand["newValue"], 'x')

def readBaud(atCommand):
	for i in range(0, len(BAUD_RATES)):
		print(" (" + str(i) + ") " + str(BAUD_RATES[i]))
	baudIndex = raw_input("Choose a baud rate [" + str(atCommand["oldValue"]) + "]: ")
	if (baudIndex == ""):
		baudIndex = atCommand["oldValue"]
	bd = int(baudIndex)
	if (bd < 0 or bd >= len(BAUD_RATES)):
		print("Invalid input\n")
		return None
	else:
		return bd

def readPan(atCommand):
	panId = raw_input("Choose a PAN ID (0 - FFFF) [" + str(atCommand["oldValue"]) + "]:")
	if (panId == ""):
		panId = atCommand["oldValue"]
	id = int("0x" + panId, 16)
	if (id < 0 or id > 0xffff):
		print("Invalid input\n")
		return None
	else:
		return id;

def readChannel(atCommand):
	channel = raw_input("Choose a Channel (B: XBee) (C: XBee Pro) [" + str(atCommand["oldValue"]) + "]: ")
	if (channel == ""):
		channel = atCommand["oldValue"]
	if (channel == 'B' or channel == 'C'):
		ch = channel
		return ch
	else:
		print("Invalid input\n")
		return None

def readCoordinator(atCommand):
	coordinator = raw_input("Choose end device (0) or coordinator (1) [" + str(atCommand["oldValue"]) + "]: ")
	if (coordinator == ""):
		coordinator = atCommand["oldValue"]
	ce = int(coordinator)
	if (ce < 0 or ce > 1):
		print("Invalid input\n")
		return None;
	else:
		return ce;
	
def readMyId(atCommand):
	for x in supportedATCommands:
		if x["at"] == "CE":
			return x["newValue"]
	return None

def readDestinationHigh(atCommand):
	return "0"

def readDestinationLow(atCommand):
	for x in supportedATCommands:
		if x["at"] == "CE":
			return x["newValue"]
	return None


BAUD_RATES = [1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200]

########## Main program starts here ##########
#Supported AT commands, along with the following info:
#	at: the AT command
#	writable: boolean indicating whether it is to be written (True) or read only (False)
#	read: a function to read the user's input into a variable of the proper type, populating newValue
#	write: a function to format the saved value into an AT command for writing.
supportedATCommands = [
		{"at": "SH", "writable": False}, # serial number high 
		{"at": "SL", "writable": False}, # serial number low 
		{"at": "BD", "writable": True, "read": readBaud, "write": writeGeneric}, # baud rate
		{"at": "CH", "writable": False}, # channel
		{"at": "ID", "writable": True, "read": readPan, "write": writePan}, # pan id
		{"at": "CE", "writable": True, "read": readCoordinator, "write": writeGeneric}, # coordinator enable
		{"at": "RR", "writable": False}, # retries
		{"at": "RO", "writable": False}, # packetization timeout
		{"at": "MY", "writable": True, "read": readMyId, "write": writeGeneric}, # my id 
		{"at": "DH", "writable": True, "read": readDestinationHigh, "write": writeGeneric}, # destination address high
		{"at": "DL", "writable": True, "read": readDestinationLow, "write": writeGeneric}, # destination address low 
		{"at": "VR", "writable": False}, # firmware version
]

if (len(sys.argv) != 2):
	print("Usage: " + sys.argv[0] + " <port>")
	sys.exit(0)

for i in range(0, len(BAUD_RATES)):
	print(" (" + str(i) + ") " + str(BAUD_RATES[i]))

baudIndex = input("Choose a baud rate: ")

ser = serial.Serial(sys.argv[1], BAUD_RATES[baudIndex], timeout=2)

if (not(enterCommandMode(ser))):
	sys.exit(-1);

print("\nCurrent configuration:")
for x in supportedATCommands:
	x["oldValue"] = getReplyFor(ser, x["at"])
	print("  " + x["at"] + ": " + x["oldValue"])

print("\n")
	
if (not(exitCommandMode(ser))):
	sys.exit(-1);

print("Read successful\n")

changeConfig = raw_input("Change configuration? (y/n): ")
if (changeConfig != "Y" and changeConfig != "y"):
	sys.exit(0)

for x in supportedATCommands:
	if x["writable"]:
		x["newValue"] = x["read"](x)

atCommandString = ""
for x in supportedATCommands:
	if x["writable"]:
		atCommand = x["write"](x)
		if atCommand != None:
			atCommandString = atCommandString + atCommand + ","

atCommandString = atCommandString + "WR"

confirm = raw_input(atCommandString + "\nOK to write? (y/n): ")
if (confirm == "y" or confirm == "Y"):
	if (not(enterCommandMode(ser))):
		sys.exit(-1);
	ser.write("ATRE\r")
	ser.flush()
	if(not(isOkay(ser))):
		print("Can't reset\n");
		sys.exit(-1);

	if getReplyFor(ser, atCommandString) != "OK":
		print("Cannot write configuration\n")
		sys.exit(-1);
	
	if (not(exitCommandMode(ser))):
		sys.exit(-1);
	
	print("Configuration update successful\n")
else:
	print("Cancelled configuration update\n")

