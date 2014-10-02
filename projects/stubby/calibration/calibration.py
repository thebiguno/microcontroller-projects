# Stubby Calibration Program
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

MODE_CALIBRATION_JOINTS = 0x01
MODE_CALIBRATION_FEET = 0x02
MODE_CALIBRATION_MAGNETOMETER = 0x03

########## Main program here ##########

def main(ser):
	while True:
		writeMessage(ser, 0x00, [0x43])
		print(
"""

Stubby Calibration: Please selection an option below:
	1) Joint Calibration
	2) X,Y,Z Foot Position Calibration
	3) Magnetometer Calibration"
	L) Load all values from EEPROM
	S) Save all values to EEPROM
	Q) Quit (Any unsaved changes will be lost)
""")
		choice = raw_input("Selected Option: ")
	
		if (choice == "1"):
			doLegCalibration(ser, MODE_CALIBRATION_JOINTS)
		elif (choice == "2"):
			doLegCalibration(ser, MODE_CALIBRATION_FEET)
		elif (choice == "3"):
			doMagnetometerCalibration(ser)
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

def doLegCalibration(ser, mode):
	requestMessage = 0x00
	sendMessage = 0x00
	if (mode == MODE_CALIBRATION_JOINTS):
		requestMessage = MESSAGE_REQUEST_JOINT_CALIBRATION
		sendMessage = MESSAGE_SEND_JOINT_CALIBRATION
	elif (mode == MODE_CALIBRATION_FEET):
		requestMessage = MESSAGE_REQUEST_FOOT_CALIBRATION
		sendMessage = MESSAGE_SEND_FOOT_CALIBRATION
	else:
		print("Invalid mode detected")
		return
		

	writeMessage(ser, requestMessage, [])
	response = readMessage(ser)
	if (response == False):
		print("Communication failure")
		return
	elif (response["command"] != sendMessage):
		print("Invalid response detected")
		return

	d = response["data"]
	while True:
		print("\nPlease select a leg to modify")
		print("	0) Front Left")
		print("	1) Middle Left")
		print("	2) Rear Left")
		print("	3) Rear Right")
		print("	4) Middle Right")
		print("	5) Front Right")
		print("	R) Reset all joint values to 0")
		print("	Q) Return to main menu")
		
		leg = raw_input("Selected Option: ")
		if (digit.match(leg) and int(leg) >= 0 and int(leg) <= 5):
			leg = int(leg)
			while True:
				if (mode == MODE_CALIBRATION_JOINTS):
					print("\nPlease select a joint to modify")
					print("	0) Tibia")
					print("	1) Femur")
					print("	2) Coxa")
					print("	Q) Return to leg selection menu")
				elif (mode == MODE_CALIBRATION_FEET):
					print("\nPlease select a foot co-ordinate to modify")
					print("	0) X")
					print("	1) Y")
					print("	2) Z")
					print("	Q) Return to leg selection menu")
					
				joint = raw_input("Selected Option: ")
				if (digit.match(joint) and int(joint) >= 0 and int(joint) <= 2):
					joint = int(joint)
					print("\nPress '+' to increment, '-' to decrement, a valid number (-128 to 127), or 'Q' to return to joint selection.")
					while True:
						key = raw_input("Selected Option (current value: " + str(to_int8_t(d[leg * 3 + int(joint)])) + "): ")

						if (key == "+"):
							d[leg * 3 + int(joint)] = to_uint8_t(to_int8_t(d[leg * 3 + int(joint)] + 1))
						elif (key == "-"):
							d[leg * 3 + int(joint)] = to_uint8_t(to_int8_t(d[leg * 3 + int(joint)] - 1))
						elif (integer.match(key) and int(key) >= -128 and int(key) <= 127):
							d[leg * 3 + int(joint)] = to_uint8_t(int(key))
						elif (key == "Q" or key == "q"):
							break;
						else:
							print("Invalid value")
						writeMessage(ser, sendMessage, d)
						
				elif (joint == "Q" or joint == "q"):
					break;
				else:
					print("Invalid joint selected.")
		elif (leg == "R" or leg == "r"):
			writeMessage(ser, MESSAGE_RESET_CALIBRATION, [mode])
		elif (leg == "Q" or leg == "q"):
			print("Exiting to main menu")
			return
		else:
			print("Invalid leg selected.")

def doMagnetometerCalibration(ser):
	while True:
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
		print("	A) Start auto magnetometer offset calibration")
		print("	M) Set manual magnetometer offsets")
		print("	Q) Return to main menu")
		
		option = raw_input("Selected Option: ")

		if (option == "C" or option == "c"):
			x = to_int16_t((d[0] << 8) + d[1])
			y = to_int16_t((d[2] << 8) + d[3])
			print("Current offsets: x=" + str(x) + ", y=" + str(y))
		elif (option == "A" or option == "a"):
			print("Stubby will now slowly turn in place, for about 3 - 5 rotations.  Please stand by...")
			writeMessage(ser, MESSAGE_START_MAGNETOMETER_CALIBRATION, [])
			xValues = []
			yValues = []
			print("Reading raw values:\nx\ty")
			while True:
				rawValue = readMessage(ser)
				if (rawValue == False):
					break
				rawData = rawValue["data"]
				xValue = to_int16_t((rawData[0] << 8) + rawData[1]);
				yValue = to_int16_t((rawData[2] << 8) + rawData[3]);
				xValues.append(xValue)
				yValues.append(yValue)
				print(str(xValue) + "\t" + str(yValue))
			xAverage = -1 * sum(xValues) / len(xValues)
			yAverage = -1 * sum(yValues) / len(yValues)
			print("Auto calculated offsets: " + str(xAverage) + ", " + str(yAverage))
			useNewOffsets = raw_input("Do you want to use these offsets? (Y/N) ")
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
	if (len(sys.argv) != 3):
		print("Usage: " + sys.argv[0] + " <port> <baud>")
		sys.exit(0)

	ser = serial.Serial(sys.argv[1], sys.argv[2], timeout=1)

	main(ser)