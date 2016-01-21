# Chiindii Calibration Program
#
# Usage: <calibration.py> <port>
# Follow the on-screen prompts
###################

import re, serial, struct, sys

########## Common variables ##########

axis = 0
#digit = re.compile('^[0-9]$')
#integer = re.compile('^-?[0-9]+$')
floatregex = re.compile('^-?[0-9.]+$')

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

#MODE_CALIBRATION_RATE_PID = 0x01
#MODE_CALIBRATION_COMPLEMENTARY = 0x02
#MODE_CALIBRATION_ANGLE_PID = 0x03

########## Main program here ##########

def main(ser):
	#We need to pick an axis before anything else
	chooseAxis(ser);
	
	while True:
		print(
"""
Chiindii Calibration: Please selection an option below:
	R) Change Rate PID calibration
	C) Change Complementary filter calibration
	A) Change Angle PID calibration
	L) Load all values from EEPROM (revert changes for this session)
	S) Save all values to EEPROM
	Q) Quit (Any unsaved changes will be lost)
""")
		choice = raw_input("Selected Option: ")
		
		if (choice == "R" or choice == "r"):
			doRatePidCalibration(ser)
		elif (choice == "C" or choice == "c"):
			if (axis == 2):
				print("Complementary filter tuning not applicable for Z axis")
			else:
				doComplementaryCalibration(ser)
		elif (choice == "A" or choice == "a"):
			if (axis == 2):
				print("Angle PID tuning not application for Z axis")
			else:
				doAnglePidCalibration(ser)
		elif (choice == "L" or choice == "l"):
			writeMessage(ser, MESSAGE_LOAD_CALIBRATION, [])
			print("All values loaded from EEPROM")
		elif (choice == "S" or choice == "s"):
			writeMessage(ser, MESSAGE_SAVE_CALIBRATION, [])
			print("All values saved to EEPROM")
		elif (choice == "Q" or choice == "q"):
			sys.exit(0)
		else:
			print("Invalid selection, please try again\n")
		
########## Primary functions here ##########

def chooseAxis(ser):
	while(True):
		print("""
Each axis must be tuned independently since Chiindii must be placed into the jig
in the proper orientation for that axis.
For Pitch and Roll, attach the arms to the jig such that the craft can freely pitch or roll.
For Yaw, suspend the craft from the top so that it can freely yaw.

Please select an axis to calibrate:
	P) Pitch (Y)
	R) Roll (X)
	Y) Yaw (Z)
	Q) Quit calibration
""")

		response = raw_input("Selected option: ")

		if (response == "Q" or response == "q"):
			sys.exit(0)
		elif (response == "P" or response == "p" or response == "R" or response == "r" or response == "Y" or response == "y"):
			## translate axis into number for indexing into tuning array
			if (response == "P" or response == "p"): axis = 0
			elif (response == "R" or response == "r"): axis = 1
			else: axis = 2
			return;
		else:
			print("Invalid parameter, please try again\n")


def doRatePidCalibration(ser, mode):
	raw_input("""
Rate PID calibration allows Chiindii to quickly an accurately achieve the requested rotational
rate of change.  Each axis (pitch, roll, yaw) has three parameters (proportional, integral, 
derivitive) for a total of nine parameters.
Tune each axis in turn by requesting a fixed rate of change (for example 1 degree / second)
and increasing the proportional parameter until the the observed rate of change starts to
occilate around the requested rate.  At that point begin to increase the integral parameter
until the observed rate of change stops occilating.  Continue tuning until the observed rate
of change is stable and matches the requested rate of change for a variety of requested rates.

Place Chiindii into the jig, and press enter.
""")
	
	writeMessage(ser, 0x00, [0x43])
	writeMessage(ser, MESSAGE_REQUEST_CALIBRATION_RATE_PID, [])
	response = readMessage(ser)
	if (response == False):
		print("Communication failure")
		return
	elif (response["command"] != MESSAGE_REQUEST_CALIBRATION_RATE_PID):
		print("Invalid response detected")
		return
	d = response["data"]
	while True:
		print("\nPlease select a parameter to modify")
		print("	S) Rate Set Point (deg/sec)")
		print("	P) Proportional")
		print("	I) Integral")
		print("	D) Derivitive")
		print("	Q) Return to axis selection")
		
		param = raw_input("Select parameter: ")
		
		if (param == "Q" or param == "q"):
			break
		elif (param == "R" or param == "r"):
			print("\nEnter a valid number, or 'Q' to return to parameter selection.")
			while True:
				value = raw_input("Rate: ")
				if (floatregex.match(value)):
					rate_sp = [0,0,0,0]
					struct.pack_info("<f", rate_sp, 0, radians(float(value)))
					writeMessage(ser, MESSAGE_SEND_ANGLE_SP, rate_sp)
				elif (value == "Q" or value == "q"):
					break;
				else:
					print("Invalid value, please try again\n")
		elif (param == "P" or param == "p" or param == "I" or param == "i" or param == "D" or param == "d"):
			## translate param into number for indexing into tuning array
			if (param == "P" or param == "p"): 
				param = 0
			elif (param == "I" or param == "i"): 
				param = 1
			else: 
				param = 2
			
			print("\nEnter a valid number, or 'Q' to return to parameter selection.")
			while True:
				value = raw_input("Value (" + str(to_float_t(d[axis * 3 + param])) + "): ")
				if (floatregex.match(value)):
					d[axis * 3 + param] = to_float_t(float(value))
					writeMessage(ser, MESSAGE_SEND_CALIBRATION_RATE_PID, d)
				elif (value == "Q" or value == "q"):
					break;
				else:
					print("Invalid value, please try again\n")
		else:
			print("Invalid axis, please try again\n")

def doAnglePidCalibration(ser, mode):
	raw_input("""
Angle PID calibration allows Chiindii to quickly an accurately achieve the requested absolute
angle.  Each axis (pitch, roll, yaw) has three parameters (proportional, integral, 
derivitive) for a total of nine parameters.
Tune each axis in turn by requesting a fixed angle (for example 30 degree)
and increasing the proportional parameter until the the observed angle starts to
occilate around the angle.  At that point begin to increase the integral parameter
until the observed angle stops occilating.  Continue tuning until the observed angle
is stable and matches the requested angle for a variety of requested angles.

Place Chiindii into the jig, and press enter.
""")
	
	writeMessage(ser, 0x00, [0x43])
	writeMessage(ser, MESSAGE_REQUEST_CALIBRATION_ANGLE_PID, [])
	response = readMessage(ser)
	if (response == False):
		print("Communication failure")
		return
	elif (response["command"] != MESSAGE_REQUEST_CALIBRATION_ANGLE_PID):
		print("Invalid response detected")
		return
	d = response["data"]
	while True:
		print("\nPlease select a parameter to modify")
		print("	S) Angle Set Point (deg)")
		print("	P) Proportional")
		print("	I) Integral")
		print("	D) Derivitive")
		print("	Q) Return to axis selection")
		
		param = raw_input("Select parameter: ")
		
		if (param == "Q" or param == "q"):
			break
		elif (param == "R" or param == "r"):
			print("\nEnter a valid number, or 'Q' to return to parameter selection.")
			while True:
				value = raw_input("Angle: ")
				if (floatregex.match(value)):
					angle_sp = [0,0,0,0]
					struct.pack_info("<f", angle_sp, 0, radians(float(value)))
					writeMessage(ser, MESSAGE_SEND_ANGLE_SP, angle_sp)
				elif (value == "Q" or value == "q"):
					break;
				else:
					print("Invalid value, please try again\n")
		elif (param == "P" or param == "p" or param == "I" or param == "i" or param == "D" or param == "d"):
			## translate param into number for indexing into tuning array
			if (param == "P" or param == "p"): 
				param = 0
			elif (param == "I" or param == "i"): 
				param = 1
			else: 
				param = 2
			
			print("\nEnter a valid number, or 'Q' to return to parameter selection.")
			while True:
				value = raw_input("Value (" + str(struct.unpack_from("<f", d, axis * 12 + param * 4)) + "): ")
				if (floatregex.match(value)):
					struct.pack_into("<f", d, axis * 12 + param * 4, value)
					writeMessage(ser, MESSAGE_SEND_CALIBRATION_ANGLE_PID, d)
				elif (value == "Q" or value == "q"):
					break;
				else:
					print("Invalid value, please try again\n")
		else:
			print("Invalid axis, please try again\n")


def doComplementaryCalibration(ser):
	print("""
Complemenentary calibration allows Chiindii to integrate Gyro and Accelerometer
data to reduce Gyro drift.  There is one paramater that called Tau which is the response
time of integration.  This allows a trade off between drift elimination and responsiveness.
The tuning allows you to collect raw and integrated data which can be graphed.
""")
	while True:
		print("\nPlease select a parameter to modify")
		print("	T) Tau")
		print("	S) Start reading live test data")
		print("	Q) Return to axis selection")
		
		param = raw_input("Selected Option: ")
		
		if (param == "Q" or param == "q"):
			break
		elif (param == "T" or param == "t"):
			while True:
				#Load the current calibration
				writeMessage(ser, MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY, [])
				response = readMessage(ser)
				if (response == False):
					print("Communication failure")
					return
				elif (response["command"] != MESSAGE_REQUEST_CALIBRATION_COMPLEMENTARY):
					print("Invalid response detected: received command " + hex(response["command"]))
					return
				d = response["data"]
				
				print("\nEnter a valid number, or 'Q' to return to parameter selection.")
				value = raw_input("Tau (" + str(round(struct.unpack_from("<f", buffer(str(bytearray(d))), axis * 4)[0], 3)) + "): ")
				if (floatregex.match(value)):
					
					bytes = struct.pack("<f", float(value))
					for i, b in enumerate(bytes):
						print i, ord(b)
						d[i + (axis * 4)] = ord(b)
					writeMessage(ser, MESSAGE_SEND_CALIBRATION_COMPLEMENTARY, d)
				elif (value == "Q" or value == "q"):
					break;
				else:
					print("Invalid value, please try again\n")
		elif (param == "S" or param == "s"):
			writeMessage(ser, MESSAGE_START_COMPLEMENTARY_CALIBRATION, [axis])
			while True:
				response = readMessage(ser)
				if (response == False):
					break
				d = buffer(str(bytearray(response["data"])))
				
				accel = struct.unpack_from("<f", d, 0)[0];
				gyro = struct.unpack_from("<f", d, 4)[0];
				comp = struct.unpack_from("<f", d, 8)[0];
				print(str(round(accel, 4)) + "\t" + str(round(gyro, 4)) + "\t" + str(round(comp, 4)))
		else:
			print("Invalid axis, please try again\n")



########## Helper functions here ##########

def writeMessage(ser, command, data):
	message = [0x7e, len(data) + 1, command]
	checksum = command
	for i in range(0, len(data)):
		message.append(data[i])
		checksum = (checksum + data[i]) & 0xFF
	checksum = 0xFF - checksum
	message.append(checksum)
	ser.write(''.join(chr(b) for b in message))

def readMessage(ser):
	START = 0x7e
	ESCAPE = 0x7d
	MAX_SIZE = 40

	err = False
	esc = False
	pos = 0
	length = 0
	cmd = 0
	chk = 0x00

	buf = [0 for i in range(254)]		#max langth of message

	while True:
		rawArray = ser.read()
		if (len(rawArray) == 0):
			return False;
		
		for raw in rawArray:
			b = ord(raw)
		
			if (err and b == START):
				# recover from error condition
				err = False
				pos = 0
			elif (err):
				continue

			if (pos > 0 and b == START):
				# unexpected start of frame
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
					continue

				if (pos == (length + 2)):
					if (chk == 0xff):
						result = {}
						result["command"] = cmd
						result["data"] = buf[:length - 1]
						result["length"] = length
						return result
					
					else:
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