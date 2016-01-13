# Chiindii Calibration Program
#
# Usage: <calibration.py> <port> <baud>
# Follow the on-screen prompts
###################

import re, serial, sys

########## Common variables ##########

digit = re.compile('^[0-9]$')
integer = re.compile('^-?[0-9]+$')

MESSAGE_SAVE_CALIBRATION = 0x30
MESSAGE_LOAD_CALIBRATION = 0x31
MESSAGE_RESET_CALIBRATION = 0x32
MESSAGE_REQUEST_JOINT_CALIBRATION = 0x33
MESSAGE_SEND_JOINT_CALIBRATION = 0x34
MESSAGE_REQUEST_FOOT_CALIBRATION = 0x35
MESSAGE_SEND_FOOT_CALIBRATION = 0x36
MESSAGE_REQUEST_MAGNETOMETER_CALIBRATION = 0x37
MESSAGE_SEND_MAGNETOMETER_CALIBRATION = 0x38
MESSAGE_START_MAGNETOMETER_CALIBRATION = 0x39

MODE_CALIBRATION_RATE_PID = 0x01
MODE_CALIBRATION_COMPLEMENTARY = 0x02
MODE_CALIBRATION_ANGLE_PID = 0x03

########## Main program here ##########

def main(ser):
	while True:
		writeMessage(ser, 0x00, [0x43])
		print(
"""
Chiindii Calibration: Please selection an option below:
	R) Rate PID Calibration
	C) Complementary Filter Calibration
	A) Attitude PID Calibration
	L) Load all values from EEPROM (revert changes for this session)
	S) Save all values to EEPROM
	Q) Quit (Any unsaved changes will be lost)
""")
		choice = raw_input("Selected Option: ")
	
		if (choice == "R" or choice == "r"):
			doRatePidCalibration(ser)
		elif (choice == "C" or choice == "c"):
			doComplementaryCalibration(ser)
		elif (choice == "A" or choice == "a"):
			doAnglePidCalibration(ser)
		elif (choice == "L" or choice == "l"):
			writeMessage(ser, MESSAGE_LOAD_CALIBRATION, [])
			print("All values loaded from EEPROM")
		elif (choice == "S" or choice == "s"):
			writeMessage(ser, MESSAGE_SAVE_CALIBRATION, [])
			print("All values saved to EEPROM")
		elif (choice == "Q" or choice == "q"):
			writeMessage(ser, 0x00, [0x00])	#Exit calibration controller mode
			sys.exit(0)
		else:
			print("Invalid selection, please try again\n")
		
########## Primary functions here ##########

def doRatePidCalibration(ser, mode):
	requestMessage = 0x00
	sendMessage = 0x00
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
		print("\nPlease select a axis to modify")
		print("	P) Pitch")
		print("	R) Roll")
		print("	Y) Yaw")
		print("	Q) Return to the main menu")
		
		axis = raw_input("Select axis: ")
		
		if (axis == "Q" or axis == "q"):
			break
		elif (axis == "P" or axis == "p" or axis == "R" or axis == "r" or axis == "Y" or axis == "y"):
			## translate axis into number for indexing into tuning array
			if (axis == "P" or axis == "p"): axis = 0
			elif (axis == "R" or axis == "r"): axis = 1
			else: axis = 2
			
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
					value = raw_input("value (" + str(to_float_t(rate_sp) + "): ")
					if (float.match(value)):
						rate_sp = to_float_t(float(value))
						writeMessage(ser, MESSAGE_SEND_RATE_SP, rate_sp)
					elif (value == "Q" or value == "q"):
						break;
					else:
						print("Invalid value, please try again\n")
			elif (param == "P" or param == "p" or param == "I" or param == "i" or param == "D" or param == "d"):
				## translate param into number for indexing into tuning array
				if (param == "P" or param == "p"): param = 0
				elif (param == "I" or param == "i"): param = 1
				else: param = 2
				
				print("\nEnter a valid number, or 'Q' to return to parameter selection.")
				while True:
					value = raw_input("value (" + str(to_float_t(d[axis * 3 + param])) + "): ")
					if (float.match(value)):
						d[axis * 3 + param] = to_float_t(float(value))
						writeMessage(ser, MESSAGE_SEND_CALIBRATION_RATE_PID, d)
					elif (value == "Q" or value == "q"):
						break;
					else:
						print("Invalid value, please try again\n")
			else
				print("Invalid parameter, please try again\n")
		else:
			print("Invalid axis, please try again\n")

def doMagnetometerCalibration(ser):
	raw_input("""
Magnetometer calibration determines what offset values are required for the magnetometer
to be properly centered.  Each magnetometer will be different, and depending on the 
magnetic fields surrounding the servos, each frame / servo combination will likely
be different as well.
The best way to calibrate the magnetometer is to use the automatic routine.  Place
Stubby on the floor or a table, far from the edge.  When calibration starts, Stubby
will slowly turn in place for a minute or two, completing about 3 - 5 full rotations.
While rotating, Stubby repeatedly reads the magnetometer, and sends the data to the 
calibration program.  When completed, the calibration program analyzes the data, and
computes the required calibration offsets.
The calibration units for this mode are offsets on the raw magnetometer readings.
See http://stubby.digitalcave.ca/stubby/calibration.jsp for additional instructions on 
calibration, including pictures.
Please place Stubby on the floor, and press enter.
""")

	while True:
		writeMessage(ser, 0x00, [0x43])
		writeMessage(ser, MESSAGE_REQUEST_MAGNETOMETER_CALIBRATION, [])
		response = readMessage(ser)
		if (response == False):
			print("Communication failure")
			return
		elif (response["command"] != MESSAGE_SEND_MAGNETOMETER_CALIBRATION):
			print("Invalid response detected")
			return
		
		d = response["data"]
	
		print("\nPlease select an option")
		print("	C) Show current magnetometer offsets")
		print("	S) Start auto magnetometer offset calibration")
		print("	M) Set manual magnetometer offsets")
		print("	Q) Return to main menu")
		
		option = raw_input("Selected Option: ")

		if (option == "C" or option == "c"):
			x = to_int16_t((d[0] << 8) + d[1])
			y = to_int16_t((d[2] << 8) + d[3])
			print("Current offsets: x=" + str(x) + ", y=" + str(y))
		elif (option == "S" or option == "s"):
			xOffset = to_int16_t((d[0] << 8) + d[1])
			yOffset = to_int16_t((d[2] << 8) + d[3])

			print("Stubby will now slowly turn in place, for about 3 - 5 rotations.  Please stand by...")
			writeMessage(ser, MESSAGE_START_MAGNETOMETER_CALIBRATION, [])
			xValues = []
			yValues = []
			print("Reading raw values and showing adjusted values with current offsets (" + str(xOffset) + ", " + str(yOffset) + "):\nX\tY\tadj. X\tadj. Y")
			while True:
				rawValue = readMessage(ser)
				if (rawValue == False):
					break
				rawData = rawValue["data"]
				xValue = to_int16_t((rawData[0] << 8) + rawData[1]);
				yValue = to_int16_t((rawData[2] << 8) + rawData[3]);
				xValues.append(xValue)
				yValues.append(yValue)
				print(str(xValue) + "\t" + str(yValue) + "\t" + str(xValue + xOffset) + "\t" + str(yValue + yOffset))
			xAverage = -1 * sum(xValues) / len(xValues)
			yAverage = -1 * sum(yValues) / len(yValues)
			print("Current offsets: " + str(xOffset) + ", " + str(yOffset))
			print("New (auto calculated) offsets: " + str(xAverage) + ", " + str(yAverage))
			useNewOffsets = raw_input("Do you want to use these new offsets? (Y/N) ")
			if (useNewOffsets == "Y" or useNewOffsets == "y"):
				#Send new calibration values
				data = []
				data.append((to_uint16_t(xAverage) >> 8) & 0xFF)
				data.append((to_uint16_t(xAverage)) & 0xFF)
				data.append((to_uint16_t(yAverage) >> 8) & 0xFF)
				data.append((to_uint16_t(yAverage)) & 0xFF)
				writeMessage(ser, MESSAGE_SEND_MAGNETOMETER_CALIBRATION, data)
			
		elif (option == "M" or option == "m"):
			xOffset = raw_input("X Offset: ")
			yOffset = raw_input("Y Offset: ")
			if (not(integer.match(xOffset)) or not(integer.match(yOffset))):
				print("Invalid offset specified.  Please enter a valid integer.")
			else:
				data = []
				data.append((to_uint16_t(int(xOffset)) >> 8) & 0xFF)
				data.append((to_uint16_t(int(xOffset))) & 0xFF)
				data.append((to_uint16_t(int(yOffset)) >> 8) & 0xFF)
				data.append((to_uint16_t(int(yOffset))) & 0xFF)
				writeMessage(ser, MESSAGE_SEND_MAGNETOMETER_CALIBRATION, data)
				
		elif (option == "Q" or option == "q"):
			print("Exiting to main menu")
			return
		else:
			print("Invalid option selected.")


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

	buf = {}

	while True:
		rawArray = ser.read();
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
						result["data"] = buf
						result["length"] = length
						return result
					
					else:
						err = True;
					pos = 0;
					chk = 0;
				else:
					buf[pos - 3] = b
					pos = pos + 1

#These next four functions are the equivalent of casting to the specified c types.
def to_int8_t(b):
	if (b > 0xFF):
		b = 0xFF
	elif (b < 0x00):
		b = 0x00
		
	if (b > 0x7F):
		return (b - 0xFF)
	else:
		return b

def to_uint8_t(b):
	if (b < 0x00):
		return (0xFF + b) & 0xFF
	else:
		return b & 0xFF
	
def to_int16_t(i):
	if (i > 0xFFFF):
		i = 0xFFFF
	elif (i < 0x0000):
		i = 0x0000
		
	if (i > 0x7FFF):
		return (i - 0xFFFF)
	else:
		return i

def to_uint16_t(i):
	if (i < 0x0000):
		return (0xFFFF + i) & 0xFFFF
	else:
		return i & 0xFFFF


########## Main startup hooks here ##########

if (__name__=="__main__"):
	if (len(sys.argv) != 2):
		print("Usage: " + sys.argv[0] + " <port>")
		sys.exit(0)

	ser = serial.Serial(sys.argv[1], 38400, timeout=1)

	main(ser)