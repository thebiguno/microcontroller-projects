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
						key = raw_input("Selected Option (current value: " + str(toSignedByte(d[leg * 3 + int(joint)])) + "): ")

						if (key == "+"):
							d[leg * 3 + int(joint)] = toUnsignedByte(toSignedByte(d[leg * 3 + int(joint)] + 1))
						elif (key == "-"):
							d[leg * 3 + int(joint)] = toUnsignedByte(toSignedByte(d[leg * 3 + int(joint)] - 1))
						elif (integer.match(key) and int(key) >= -128 and int(key) <= 127):
							d[leg * 3 + int(joint)] = toUnsignedByte(int(key))
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

def toSignedByte(b):
	if (b > 255):
		b = 256
	elif (b < 0):
		b = 0
		
	if (b > 127):
		return (b - 256)
	else:
		return b

def toUnsignedByte(b):
	if (b < 0):
		return (256 + b) & 0xFF
	else:
		return b & 0xFF

########## Main startup hooks here ##########

if (__name__=="__main__"):
	if (len(sys.argv) != 3):
		print("Usage: " + sys.argv[0] + " <port> <baud>")
		sys.exit(0)

	ser = serial.Serial(sys.argv[1], sys.argv[2], timeout=1)

	main(ser)