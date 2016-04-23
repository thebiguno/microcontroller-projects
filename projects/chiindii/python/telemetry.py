# Chiindii Calibration Program
#
# Usage: <calibration.py> <port>
# Follow the on-screen prompts
###################

import math, re, serial, struct, sys, threading, time

def readMessages(ser):
	while True:
		message = readNextMessage(ser)
		if (message != False):
			if (message["command"] == 0x24):
				#print("{0[0]:X},{0[1]:X},{0[2]:X},{0[3]:X}	{0[4]:X},{0[5]:X},{0[6]:X},{0[7]:X}	{0[8]:X},{0[9]:X},{0[10]:X},{0[11]:X}".format(message["data"]))
				#print("{0[0]:.3f}	{0[1]:.3f}	{0[2]:.3f}".format(struct.unpack("<fff", buffer(str(bytearray(message["data"]))))))
				#print(time.strftime("%M:%S") + ": {0[0]}	{0[1]}	{0[2]}	{0[3]}	{1[1]:02X}{1[0]:02X}	{1[3]:02X}{1[2]:02X}	{1[5]:02X}{1[4]:02X}	{1[7]:02X}{1[6]:02X}".format(struct.unpack("<hhhh", buffer(str(bytearray(message["data"])))), message["data"]))
				print(time.strftime("%M:%S") + ": {0[0]}	{0[1]}	{0[2]}	{0[3]}".format(struct.unpack("<hhhh", buffer(str(bytearray(message["data"])))), message["data"]))
	
def readNextMessage(ser):
	START = 0x7e
	ESCAPE = 0x7d
	MAX_SIZE = 255

	err = False
	esc = False
	pos = 0
	length = 0
	cmd = 0
	chk = 0x00

	buf = [0 for i in range(MAX_SIZE)]		#max langth of message

	while True:
		rawArray = ser.read()
		if (len(rawArray) == 0):
			return False;
		
		for raw in rawArray:
			b = ord(raw)
			#print(hex(b))
		
			if (pos == 0 and b != START):
				print("Garbage data, ignoring")
				continue
			
			if (err):
				if (b == START):
					# recover from error condition
					err = False
					print("Recover from error")
					pos = 0
				else:
					#print("In Error State")
					continue

			if (pos > 0 and b == START):
				# unexpected start of frame
				print("Unexpected start of frame")
				err = True
				continue

			if (pos > 0 and b == ESCAPE):
				# unescape next byte
				esc = True
				continue
		
			if (esc):
				# unescape current byte
				b = 0x20 ^ b
				esc = False

			if (pos > 1):
				chk = (chk + b) & 0xFF
		
			if (pos == 0):
				pos = pos + 1
				continue
			elif (pos == 1):
				if (b == 0):
					error = True
					print("Invalid length")
				else:
					length = b
					pos = pos + 1
				continue
			elif (pos == 2):
				cmd = b
				pos = pos + 1
				continue
			else:
				if ((pos - 3)> MAX_SIZE):
					# this probably can't happen
					print("Overlength message")
					continue

				if (pos == (length + 2)):
					if (chk == 0xff):
						result = {}
						result["command"] = cmd
						result["data"] = buf[:length - 1]
						result["length"] = length
						return result
					
					else:
						print("Invalid checksum")
						err = True;
					pos = 0;
					chk = 0;
				else:
					buf[pos - 3] = b
					pos = pos + 1


########## Main startup hooks here ##########

if (__name__=="__main__"):
	if (len(sys.argv) != 2):
		print("Usage: " + sys.argv[0] + " <port>")
		sys.exit(0)

	ser = serial.Serial(sys.argv[1], 38400, timeout=0.1)
	readMessages(ser)