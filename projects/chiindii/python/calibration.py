# Chiindii Calibration Program
#
# Usage: <calibration.py> <port>
# Follow the on-screen prompts
###################

import math, re, time, serial, struct, sys

########## Common variables ##########

axis = -1
axisString = ""
#digit = re.compile('^[0-9]$')
integerregex = re.compile('^-?[0-9]+$')
floatregex = re.compile('^-?[0-9.]+$')

MESSAGE_ARMED = 0x20
MESSAGE_THROTTLE = 0x21
MESSAGE_RATE = 0x22
MESSAGE_ANGLE = 0x23

MESSAGE_ANNOUNCE_CONTROL_ID = 0x00
MESSAGE_SEND_ACKNOWLEDGE = 0x01
MESSAGE_SEND_COMPLETE = 0x02
MESSAGE_REQUEST_DEBUG = 0x03
MESSAGE_DEBUG = 0x05
MESSAGE_REQUEST_BATTERY = 0x06
MESSAGE_SEND_BATTERY = 0x07

MESSAGE_SAVE_CALIBRATION = 0x30
MESSAGE_LOAD_CALIBRATION = 0x31
MESSAGE_REQUEST_CALIBRATION_RATE_PID = 0x33
MESSAGE_SEND_CALIBRATION_RATE_PID = 0x34
MESSAGE_REQUEST_CALIBRATION_ANGLE_PID = 0x35
MESSAGE_SEND_CALIBRATION_ANGLE_PID = 0x36
MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY = 0x37
MESSAGE_SEND_CALIBRATION_COMPLEMENTARY = 0x38
MESSAGE_START_COMPLEMENTARY_CALIBRATION = 0x39
MESSAGE_SEND_TUNING_DATA = 0x3A
MESSAGE_START_MPU_CALIBRATION = 0x3B

#MODE_CALIBRATION_RATE_PID = 0x01
#MODE_CALIBRATION_COMPLEMENTARY = 0x02
#MODE_CALIBRATION_ANGLE_PID = 0x03

MODE_UNARMED = 0x00
MODE_ARMED_ANGLE = 0x01
MODE_ARMED_RATE = 0x02


########## Main program here ##########

def main(ser):
	#We need to pick an axis before anything else
	chooseAxis(ser);
	if (axis == -1):
		print("Axis selection is required.")
		sys.exit(0);
	
	while True:
		print(
"""
Chiindii Calibration: Please selection an option below:
	C) Change Complementary filter calibration
	R) Change Rate PID calibration
	A) Change Angle PID calibration
	V) Level MPU
	X) Change Axis
	D) Read debug messages
	L) Load all values from EEPROM (revert changes for this session)
	S) Save all values to EEPROM
	Q) Quit (Any unsaved changes will be lost)
""")
		choice = raw_input("Selected Option: ").lower()
		
		if (choice == "r"):
			doRatePidCalibration(ser)
		elif (choice == "c"):
			if (axis == 2):
				print("Complementary filter tuning not applicable for Z axis")
			else:
				doComplementaryCalibration(ser)
		elif (choice == "a"):
			doAnglePidCalibration(ser)
		elif (choice == "v"):
			doLevelMpu(ser)
		elif (choice == "x"):
			chooseAxis(ser)
		elif (choice == "d"):
			while True:
				response = readMessage(ser, MESSAGE_DEBUG)
				if (response != False):
					print("".join(map(chr, response["data"])))

		elif (choice == "l"):
			writeMessage(ser, MESSAGE_LOAD_CALIBRATION, [])
			print("All values loaded from EEPROM")
		elif (choice == "s"):
			writeMessage(ser, MESSAGE_SAVE_CALIBRATION, [])
			print("All values saved to EEPROM")
		elif (choice == "q"):
			sys.exit(0)
		else:
			print("Invalid selection, please try again\n")
		
########## Primary functions here ##########

def setAxis(value):
	global axis
	global axisString
	
	## translate axis into number for indexing into tuning array
	if (value == "x"): 
		axis = 0
		axisString = "Roll"
	elif (value == "y"): 
		axis = 1
		axisString = "Pitch"
	else: 
		axis = 2
		axisString = "Yaw"

def chooseAxis(ser):
	while(True):
		print("""
Each axis must be tuned independently since Chiindii must be placed into the jig
in the proper orientation for that axis.
For Pitch and Roll, attach the arms to the jig such that the craft can freely pitch or roll.
For Yaw, suspend the craft from the top so that it can freely yaw.

Please select an axis to calibrate:
	X) Roll
	Y) Pitch
	Z) Yaw
	Q) Exit
""")

		response = raw_input("Selected option: ").lower()

		if (response == "q"):
			sys.exit(0)
		elif (response == "x" or response == "y" or response == "z"):
			setAxis(response)
			return;
		else:
			print("Invalid parameter, please try again\n")

def doLevelMpu(ser):
	while(True):
		print("""
The MPU (Accel + Gyro) needs to be calibrated to ensure that it correctly can detect
level orientation.  Please place the craft on a solid, perfectly level surface and
choose 'V' to mark as level.  Be sure to save your calibration when completed.

Please select an option:
	V) Mark current orientation as level
	Q) Cancel
""")

		response = raw_input("Selected option: ").lower()

		if (response == "v"):
			writeMessage(ser, MESSAGE_START_MPU_CALIBRATION, [])
			return
		elif (response == "q"):
			return
		else:
			print("Invalid parameter, please try again\n")


def doComplementaryCalibration(ser):
	print("""
Complemenentary calibration allows Chiindii to integrate Gyro and Accelerometer
data to reduce Gyro drift.  There is one paramater that called Tau which is the response
time of integration.  This allows a trade off between drift elimination and responsiveness.
The tuning allows you to collect raw and integrated data which can be graphed.
""")
	while True:
		param = raw_input("""
Please select a parameter to modify
	T) Tau
	D) Start reading live attitude data
	L) Load all values from EEPROM (revert changes for this session)
	S) Save all values to EEPROM
	Q) Return to axis selection
		
Selected Option: """).lower()
		
		if (param == "q"):
			break
		elif (param == "l"):
			writeMessage(ser, MESSAGE_LOAD_CALIBRATION, [])
			print("All values loaded from EEPROM")
		elif (param == "s"):
			writeMessage(ser, MESSAGE_SAVE_CALIBRATION, [])
			print("All values saved to EEPROM")
		elif (param == "t"):
			while True:
				#Load the current calibration
				writeMessage(ser, MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY, [])
				response = readMessage(ser, MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY)
				if (response == False):
					return
				d = response["data"]
				
				print("\nEnter a valid number, X/Y to change axis, or enter to return to parameter selection.")
				value = raw_input("Current axis: {0}.  Tau ({1:.3f}): ".format(axisString, struct.unpack_from("<f", buffer(str(bytearray(d))), axis * 4)[0])).lower()
				if (value == "x" or value == "y"):
					setAxis(value)
				elif (floatregex.match(value)):
					bytes = struct.pack("<f", float(value))
					for i, b in enumerate(bytes):
						print i, ord(b)
						d[i + (axis * 4)] = ord(b)
					writeMessage(ser, MESSAGE_SEND_CALIBRATION_COMPLEMENTARY, d)
				elif (value == ""):
					break;
				else:
					print("Invalid value, please try again\n")
		elif (param == "d"):
			writeMessage(ser, MESSAGE_START_COMPLEMENTARY_CALIBRATION, [axis])
			print("Gyro\tAccel\tComp")
			while True:
				response = readMessage(ser, MESSAGE_SEND_TUNING_DATA)
				if (response == False):
					break
				print("{0[0]:.3f}\t{0[1]:.3f}\t{0[2]:.3f}".format(struct.unpack("<fff", buffer(str(bytearray(response["data"]))))))
		else:
			print("Invalid axis, please try again\n")



def doRatePidCalibration(ser):
	print("""
Rate PID calibration allows Chiindii to quickly an accurately achieve the requested rotational
rate of change.  Each axis (pitch, roll, yaw) has three parameters (proportional, integral, 
derivitive) for a total of nine parameters.
Tune each axis in turn by requesting a fixed rate of change (for example 1 degree / second)
and increasing the proportional parameter until the the observed rate of change starts to
occilate around the requested rate.  At that point begin to increase the integral parameter
until the observed rate of change stops occilating.  Continue tuning until the observed rate
of change is stable and matches the requested rate of change for a variety of requested rates.

Ensure that Chiindii is in the tuning jig.
""")
	
	writeMessage(ser, 0x00, [0x43])
	writeMessage(ser, MESSAGE_REQUEST_CALIBRATION_RATE_PID, [])
	response = readMessage(ser, MESSAGE_REQUEST_CALIBRATION_RATE_PID)
	if (response == False):
		return
	d = response["data"]
	while True:
		param = raw_input("""
Please select a parameter to modify
	R) Rate Set Point
	T) Change PID Tunings
	L) Load all values from EEPROM (revert changes for this session)
	S) Save all values to EEPROM
	Q) Return to main menu
		
Select parameter: """).lower()
		
		if (param == "q"):
			break
		elif (param == "l"):
			writeMessage(ser, MESSAGE_LOAD_CALIBRATION, [])
			print("All values loaded from EEPROM")
		elif (param == "s"):
			writeMessage(ser, MESSAGE_SAVE_CALIBRATION, [])
			print("All values saved to EEPROM")
		elif (param == "r"):
			while True:
				value = raw_input("Current axis: " + axisString + ".  Enter a rate set point (in deg/sec), X/Y/Z to change axis, or hit enter to return to parameter selection: ").lower()
				if (value == "x" or value == "y" or value == "z"):
					setAxis(value)
				elif (value == ""):
					break;
				elif (floatregex.match(value)):
					rate_sp = [0,0,0,0, 0,0,0,0, 0,0,0,0]	#3 floats
					
					#First we write three zeros to the packet
					bytes = struct.pack("<f", 0)
					for i, b in enumerate(bytes):
						rate_sp[i + 0] = ord(b)
						rate_sp[i + 4] = ord(b)
						rate_sp[i + 8] = ord(b)

					#Then we overwrite the selected axis with the current rate
					bytes = struct.pack("<f", math.radians(float(value)))
					for i, b in enumerate(bytes):
						rate_sp[i + (axis * 4)] = ord(b)

					throttle_sp = [0,0,0,0]
					throttle = struct.pack("<f", 0.25)
					for i, b in enumerate(throttle):
						throttle_sp[i] = ord(b)
					
					writeMessage(ser, MESSAGE_THROTTLE, throttle_sp)						#Set throttle
					writeMessage(ser, MESSAGE_RATE, rate_sp)								#Set rate
					writeMessage(ser, MESSAGE_ARMED, [MODE_ARMED_RATE], flush=False)		#Armed in rate mode
					
					#Show debug data...
					for i in range(10):
						writeMessage(ser, MESSAGE_ARMED, [MODE_ARMED_RATE], flush=False)		#Armed in rate mode
						time.sleep(0.5)
						

					while True:
						response = readMessage(ser, MESSAGE_DEBUG)
						if (response == False):
							break
						print("".join(map(chr, response["data"])))

				else:
					print("Invalid value, please try again\n")
		elif (param == "t"):
			param = 0
			paramString = "P"
			
			while True:
				#Load the current calibration
				writeMessage(ser, MESSAGE_REQUEST_CALIBRATION_RATE_PID, [])
				response = readMessage(ser, MESSAGE_REQUEST_CALIBRATION_RATE_PID)
				if (response == False):
					return
				d = response["data"]
				
				print("""
Current		P	I	D
Roll (X)	{0[0]:.3f}	{0[1]:.3f}	{0[2]:.3f}
Pitch (Y)	{0[3]:.3f}	{0[4]:.3f}	{0[5]:.3f}
Yaw (Z)		{0[6]:.3f}	{0[7]:.3f}	{0[8]:.3f}
(Changing axis '{1}', parameter '{2}')
""".format(struct.unpack("<fffffffff", buffer(str(bytearray(d)))), axisString, paramString))
			
				value = raw_input("Enter a valid number, X/Y/Z to change axis, P/I/D to change parameter, or enter to return to parameter selection: ").lower()
				if (value == "x" or value == "y" or value == "z"):
					setAxis(value)
				elif (value == "p" or value == "i" or value == "d"):
					# translate param into number for indexing into tuning array
					if (value == "p"): 
						param = 0
						paramString = "P"
					elif (value == "i"): 
						param = 1
						paramString = "I"
					else: 
						param = 2
						paramString = "D"
				elif (value == ""):
					break;
				elif (floatregex.match(value)):
					bytes = struct.pack("<f", float(value))
					for i, b in enumerate(bytes):
						#print i, ord(b)
						d[i + (axis * 12) + (param * 4)] = ord(b)
					writeMessage(ser, MESSAGE_SEND_CALIBRATION_RATE_PID, d)
				else:
					print("Invalid value, please try again\n")
		else:
			print("Invalid value, please try again\n")

def doAnglePidCalibration(ser):
	print("""
Angle PID calibration allows Chiindii to quickly an accurately achieve the requested absolute
angle.  Each axis (pitch, roll, yaw) has three parameters (proportional, integral, 
derivitive) for a total of nine parameters.
Tune each axis in turn by requesting a fixed angle (for example 30 degree)
and increasing the proportional parameter until the the observed angle starts to
occilate around the angle.  At that point begin to increase the integral parameter
until the observed angle stops occilating.  Continue tuning until the observed angle
is stable and matches the requested angle for a variety of requested angles.

Ensure that Chiindii is in the tuning jig.
""")
	
	writeMessage(ser, 0x00, [0x43])
	writeMessage(ser, MESSAGE_REQUEST_CALIBRATION_ANGLE_PID, [])
	response = readMessage(ser, MESSAGE_REQUEST_CALIBRATION_ANGLE_PID)
	if (response == False):
		return
	d = response["data"]
	while True:
		param = raw_input("""
Please select a parameter to modify
	A) Angle Set Point
	T) Change PID Tunings
	L) Load all values from EEPROM (revert changes for this session)
	S) Save all values to EEPROM
	Q) Return to main menu
		
Select parameter: """).lower()
		
		if (param == "q"):
			break
		elif (param == "l"):
			writeMessage(ser, MESSAGE_LOAD_CALIBRATION, [])
			print("All values loaded from EEPROM")
		elif (param == "s"):
			writeMessage(ser, MESSAGE_SAVE_CALIBRATION, [])
			print("All values saved to EEPROM")
		elif (param == "a"):
			while True:
				if (axisString == "Yaw"):
					value = raw_input("Current axis: G-Force.  Enter a hover set point (in G), X/Y/Z to change axis, or hit enter to return to parameter selection: ").lower()
				else:
					value = raw_input("Current axis: " + axisString + ".  Enter an angle set point (in deg), X/Y/Z to change axis, or hit enter to return to parameter selection: ").lower()
				if (value == "x" or value == "y" or value == "z"):
					setAxis(value)
				elif (value == ""):
					break;
				elif (floatregex.match(value)):
					angle_sp = [0,0,0,0, 0,0,0,0, 0,0,0,0]	#2 floats
				
					#First we write three zeros to the packet
					bytes = struct.pack("<f", 0)
					for i, b in enumerate(bytes):
						angle_sp[i + 0] = ord(b)
						angle_sp[i + 4] = ord(b)
						angle_sp[i + 8] = ord(b)

					#Then we overwrite the selected axis with the current rate
					bytes = struct.pack("<f", math.radians(float(value)))
					for i, b in enumerate(bytes):
						angle_sp[i + (axis * 4)] = ord(b)

					gforce = struct.pack("<f", 1.1)
					for i, b in enumerate(gforce):
						angle_sp[i + 8] = ord(b)
				
					writeMessage(ser, MESSAGE_ANGLE, angle_sp)								#Set angle
					writeMessage(ser, MESSAGE_ARMED, [MODE_ARMED_ANGLE], flush=False)		#Armed in angle mode
					
					#Show debug data...
					for i in range(10):
						writeMessage(ser, MESSAGE_ARMED, [MODE_ARMED_ANGLE], flush=False)		#Armed in rate mode
						time.sleep(0.5)
						

					while True:
						response = readMessage(ser, MESSAGE_DEBUG)
						if (response == False):
							break
						print("".join(map(chr, response["data"])))
				else:
					print("Invalid value, please try again\n")
		elif (param == "t"):
			param = 0
			paramString = "P"
			
			while True:
				#Load the current calibration
				writeMessage(ser, MESSAGE_REQUEST_CALIBRATION_ANGLE_PID, [])
				response = readMessage(ser, MESSAGE_REQUEST_CALIBRATION_ANGLE_PID)
				if (response == False):
					return
				d = response["data"]

				adjustedAxisString = axisString
				if (adjustedAxisString == "Yaw"):
					adjustedAxisString = "G-Force"
				print("""
Current		P	I	D
Roll (X)	{0[0]:.3f}	{0[1]:.3f}	{0[2]:.3f}
Pitch (Y)	{0[3]:.3f}	{0[4]:.3f}	{0[5]:.3f}
G-Force (Z)	{0[6]:.3f}	{0[7]:.3f}	{0[8]:.3f}
(Changing axis '{1}', parameter '{2}')
""".format(struct.unpack("<fffffffff", buffer(str(bytearray(d)))), adjustedAxisString, paramString))
			
				value = raw_input("Enter a valid number, X/Y/Z to change axis, P/I/D to change parameter, or enter to return to parameter selection: ").lower()
				if (value == "x" or value == "y" or value == "z"):
					setAxis(value)
				elif (value == "p" or value == "i" or value == "d"):
					# translate param into number for indexing into tuning array
					if (value == "p"): 
						param = 0
						paramString = "P"
					elif (value == "i"): 
						param = 1
						paramString = "I"
					else: 
						param = 2
						paramString = "D"
				elif (value == ""):
					break;
				elif (floatregex.match(value)):
					bytes = struct.pack("<f", float(value))
					for i, b in enumerate(bytes):
						#print i, ord(b)
						d[i + (axis * 12) + (param * 4)] = ord(b)
					writeMessage(ser, MESSAGE_SEND_CALIBRATION_ANGLE_PID, d)
				else:
					print("Invalid value, please try again\n")
		else:
			print("Invalid value, please try again\n")


########## Helper functions here ##########

def escapeByte(message, byte):
	if (byte  == 0x7D or byte == 0x7E):
		message.append(0x7D);
		message.append(byte ^ 0x20);
	else:
		message.append(byte)


def writeMessage(ser, command, data, flush=True):
	#Flush input buffer
	if (flush):
		incoming = readNextMessage(ser)
		while incoming != False:
			if (incoming["command"] == MESSAGE_DEBUG):
				print("Flushing: received debug message: " + "".join(map(chr, incoming["data"])))
			else:
				print("Flushing: received command " + hex(incoming["command"]))
			incoming = readNextMessage(ser)
	message = [0x7e, len(data) + 1, command]
	checksum = command
	for i in range(0, len(data)):
		escapeByte(message, data[i])
		checksum = (checksum + data[i]) & 0xFF
	checksum = 0xFF - checksum
	escapeByte(message, checksum)
	ser.write(''.join(chr(b) for b in message))
	print(' '.join(hex(b) for b in message))

def readMessage(ser, command):
	for i in range(5):
		message = readNextMessage(ser)
		if (message != False and message["command"] == command):
			return message
		elif (message != False and message["command"] == MESSAGE_DEBUG and command != MESSAGE_DEBUG):
			print("Received debug message: " + "".join(map(chr, message["data"])))
		elif (message != False):
			print("Received command " + hex(message["command"]))
	print("Communication failure...")
	return False
	
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
		
			if (err and b == START):
				# recover from error condition
				err = False
				#print("Recover from error")
				pos = 0
			elif (err):
				print("Error")
				continue

			if (pos > 0 and b == START):
				# unexpected start of frame
				#print("Unexpected start of frame")
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
				length = b
				pos = pos + 1
				continue
			elif (pos == 2):
				cmd = b
				pos = pos + 1
				continue
			else:
				if (pos > MAX_SIZE):
					# this probably can't happen
					#print("Overlength message")
					continue

				if (pos == (length + 2)):
					if (chk == 0xff):
						result = {}
						result["command"] = cmd
						result["data"] = buf[:length - 1]
						result["length"] = length
						return result
					
					else:
						#print("Invalid checksum")
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

	ser = serial.Serial(sys.argv[1], 38400, timeout=1)

	main(ser)