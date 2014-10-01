# Stubby Calibration Program
#
# Usage: <calibration.py> <port> <baud>
# Follow the on-screen prompts
###################

import serial, sys
from time import sleep


########## Main program here ##########

def main(ser):
	while True:
		print(
"""Stubby Calibration: Please pick a numbered option below:
	1) Joint Calibration
	2) X,Y,Z Foot Position Calibration
	3) Magnetometer Calibration"
	9) Save Changes (Persist to EEPROM)
	0) Quit
""")
		choice = input("Selected Option: ")
	
		if (choice == 1):
			doJointCalibration()
		elif (choice == 2):
			doFootCalibration()
		elif (choice == 3):
			doMagnetometerCalibration()
		elif (choice == 9):
			doSave()
		elif (choice == 0):
			sys.exit(0)
		else:
			print("Invalid selection, please try again\n")
		
########## Primary functions here ##########

def doJointCalibration(ser):
		writeMessage(ser, MESSAGE_REQUEST_JOINT_CALIBRATION, [])
		response = readMessage(ser)
		if (response["command"] != MESSAGE_SEND_JOINT_CALIBRATION):
			print("Invalid response detected")

	while True:
		for (l in range(0, 6)):
			print("Leg " + str(l))
	
			leg = input("Please select a leg to modify (or 'Q' to return to main menu): ")
			if (leg >= 0 and leg <= 5):
				while True:
					print(" (0) Tibia: " + str(response["data"][l * 3 + 0]))
					print(" (1) Femur: " + str(response["data"][l * 3 + 1]))
					print(" (2) Coxa : " + str(response["data"][l * 3 + 2]))

					joint = input("Please select a joint to modify (or 'Q' to return to main menu): ")
					if (joint >= 0 and leg <= 2):
			
					elif (joint == "Q" or joint == "q"):
						return
					else:
						print("Invalid joint selected.")
						
			elif (leg == "Q" or leg == "q"):
				print("Exiting to main menu")
				return
			else:
				print("Invalid leg selected.")

########## Helper functions here ##########

def writeMessage(ser, command, data):
	ser.write(0x7e)
	ser.write(len(data) + 1)
	ser.write(command)
	checksum = command
	for (i in range(0, len(data))):
		ser.write(data[i])
		checksum = (checksum ^ data[i]) & 0xFF
	ser.write(checksum)

def readMessage(ser):
	while True:
		b = ord(ser.read())
	
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

########## Main startup hooks here ##########

if (__name__=="__main__"):
	if (len(sys.argv) != 3):
		print("Usage: " + sys.argv[0] + " <port> <baud>")
		sys.exit(0)

	ser = serial.Serial(sys.argv[1], sys.argv[2])

	main(ser)