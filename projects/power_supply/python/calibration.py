#!/usr/bin/python
# Calibrate and configure the power supply
#############################################################################
import hid, re, struct, traceback, time

def send_generic_message_with_response(dev, message, body = None):
	tx_buffer = [chr(message)]
	if body != None:
		tx_buffer = tx_buffer + body
	if len(tx_buffer) == 1:
		tx_buffer.append(chr(0))
	#print("Sending Message: " + ''.join(hex(ord(x)) + "," for x in tx_buffer))
	dev.write(''.join(tx_buffer))

	rx_buffer = dev.read(16, 1000)
	if len(rx_buffer) == 0:
		raise Exception("Unable to read data")
	#print("Received Message: " + ''.join(hex(ord(x)) + "," for x in rx_buffer))
	return rx_buffer

def send_channel_info_message_with_response(dev, channel):
	tx_buffer = [chr(MESSAGE_CHANNEL_INFO), chr(channel)]
	#print("Sending Message: " + ''.join(hex(ord(x)) + "," for x in tx_buffer))
	dev.write(''.join(tx_buffer))

	rx_buffer = dev.read(16, 1000)
	if len(rx_buffer) == 0:
		raise Exception("Unable to read data")
	print("Received Message: " + ''.join(hex(ord(x)) + "," for x in rx_buffer))
	result = [ord(rx_buffer[0]), ord(rx_buffer[1]), struct.unpack(">h", ''.join(chr(ord(x)) for x in rx_buffer[2:4]))[0], struct.unpack(">h", ''.join(chr(ord(x)) for x in rx_buffer[4:6]))[0]]
	return result

	
def send_calibration_message_with_response(dev, message, channel, target, index, dac, adc, measured):
	tx_buffer = [chr(message), chr(channel), chr(target), chr(index)] + list(struct.pack(">H", dac)) + list(struct.pack(">H", adc)) + list(struct.pack(">h", measured))
	#print("Sending Message: " + ''.join(hex(ord(x)) + "," for x in tx_buffer))
	dev.write(''.join(tx_buffer))

	rx_buffer = dev.read(16, 1000)
	if len(rx_buffer) == 0:
		raise Exception("Unable to read data")
	print("Received Message: " + ''.join(hex(ord(x)) + "," for x in rx_buffer))
	result = [ord(rx_buffer[0]), ord(rx_buffer[1]), ord(rx_buffer[2]), ord(rx_buffer[3]), struct.unpack(">H", ''.join(chr(ord(x)) for x in rx_buffer[4:6]))[0], struct.unpack(">H", ''.join(chr(ord(x)) for x in rx_buffer[6:8]))[0], struct.unpack(">h", ''.join(chr(ord(x)) for x in rx_buffer[8:10]))[0]]
	return result;
	
def send_measurement_message_with_response(dev, message, channel=None, voltage=None, current=None):
	tx_buffer = [chr(message)]
	if channel != None:
		tx_buffer.append(chr(channel))
	if voltage != None:
		tx_buffer.append(chr((voltage >> 8) & 0xFF))
		tx_buffer.append(chr(voltage & 0xFF))
	if current != None:
		tx_buffer.append(chr((current >> 8) & 0xFF))
		tx_buffer.append(chr(current & 0xFF))
	if len(tx_buffer) == 1:
		tx_buffer.append(chr(0))
	print("Sending Message: " + ''.join(hex(ord(x)) + "," for x in tx_buffer))
	dev.write(''.join(tx_buffer))

	rx_buffer = dev.read(16, 1000)
	if len(rx_buffer) == 0:
		raise Exception("Unable to read data")
	#print("Received Message: " + ''.join(hex(ord(x)) + "," for x in rx_buffer))
	result = [ord(rx_buffer[0])]
	if len(rx_buffer) > 1:
		result.append(ord(rx_buffer[1]))
	if len(rx_buffer) > 3:
		result.append(ord(rx_buffer[2]) * 256 + ord(rx_buffer[3]))
	if len(rx_buffer) > 5:
		result.append(ord(rx_buffer[4]) * 256 + ord(rx_buffer[5]))
	return result;


########## Calibrate Voltage ##########

def calibrate_voltage(channel):
	
	channel_info = send_channel_info_message_with_response(dev, channel)
	print(channel_info)
	
	#Set voltage as low as possible, and current limiting off (let anything through)
	dac_raw = 0
	send_measurement_message_with_response(dev, MESSAGE_CHANGE_SETPOINT_RAW, channel, dac_raw, 0xFFF)
	
	if channel_info[2] > 0:
		CALIBRATION_VOLTAGES = [0, 1800, 2500, 3300, 5000, 8000, 10000, 12500]
	else:
		CALIBRATION_VOLTAGES = [0, -1800, -2500, -3300, -5000, -8000, -10000, -12500]

	for calibration_voltage in CALIBRATION_VOLTAGES:
		print("""Adjust measured voltage to exactly """ + str(calibration_voltage) + """ mV.  Hit enter when target reached.
Enter a DAC value as an integer between 0 and 4095, or 'Q' to exit calibration""")

		while True:
			response = raw_input("DAC Value for " + str(calibration_voltage) + "mV: ")
			if response == "":
				adc_raw = send_measurement_message_with_response(dev, MESSAGE_ACTUAL_RAW, channel)[2] & 0x03FF
				break;
			elif response == "Q" or response == "q":
				return;
			elif int(response) >= 0 and int(response) <= 4095:
				dac_raw = int(response)
			else:
				print("Invalid option.")

			dac_raw = send_measurement_message_with_response(dev, MESSAGE_CHANGE_SETPOINT_RAW, channel, dac_raw, 0xFFF)[2] & 0x0FFF
			print("Voltage setpoint: " + str(dac_raw))
		
		index = CALIBRATION_VOLTAGES.index(calibration_voltage)
		adc_raw = send_measurement_message_with_response(dev, MESSAGE_ACTUAL_RAW, channel)[2] & 0x03FF
		response = raw_input("""
Calibration for channel """ + str(channel + 1) + """, index """ + str(index) + """:
Measured value: """ + str(calibration_voltage) + """
ADC value: """ + str(adc_raw) + """
DAC value: """ + str(dac_raw) + """

Do you want to save these calibration values to EEPROM?
Y/N: """)
		if response == "Y" or response == "y":
			send_calibration_message_with_response(dev, MESSAGE_SET_CALIBRATION, channel, TARGET_VOLTAGE, CALIBRATION_VOLTAGES.index(calibration_voltage), dac_raw, adc_raw, calibration_voltage)
			print("Calibration Saved")
		elif response == "N" or response == "n":
			print("Calibration Discarded.")


########## Calibrate Current ##########

def calibrate_current():
	print("Unimplemented")

########## Configure DAC Addresses ##########

def configure_dac_address():
	while True:
		response = raw_input("""
Configure the DAC I2C addresses.

0-2) Choose the current I2C address
Q) Quit

Enter a menu option: """)

		if int(response) >= 0 and int(response) <= 2:
			old_address = int(response)
		elif response == "Q" or response == "q":
			break;

		response = raw_input("""
0-2) Choose the new I2C address
Q) Quit

Enter a menu option: """)
		if int(response) >= 0 and int(response) <= 2:
			new_address = int(response)
		elif response == "Q" or response == "q":
			break;

		raw_input("""Connect PORTB0 (Encoder 1A) to the I2C_PROG header of the target DAC with a jumper wire.
It may be a good idea to watch the procedure with a logic probe to verify correct operation.
Press enter when ready, or 'Q' to cancel...""")
		if response != "Q" and response != "q":
			send_generic_message_with_response(dev, MESSAGE_CONFIGURE_DAC_ADDRESS, [chr(old_address), chr(new_address)])
			print("The DAC should now have the new address, but you need to verify.")


########## Configure AREF ##########

def configure_aref():
	while True:
		response = raw_input("""
Is the AREF jumper set to the DAC or VCC?  If you have less than 6 channels and have DAC2 populated, it is
recommended to use the DAC for AREF to provide a stable reference voltage.

D) Use DAC for AREF value (DAC 2 Channel 3, or ISET5)
V) Use VCC for AREF
Q) Go back

Enter a menu option: """)
		if response == "D" or response == "d":
			send_generic_message_with_response(dev, MESSAGE_CONFIGURE_AREF, [chr(2), chr(3)])
			break;
		elif response == "V" or response == "v":
			break;		#No need to do anything here, the DAC's EEPROM will be overwritten when calibrating the associated channel
		elif response == "Q" or response == "q":
			break;


########## Calibration Menu ##########

def calibrate_menu(channel):
	while True:
		#channel_info = send_generic_message_with_response(dev, MESSAGE_CHANNEL_INFO)
		
		#CALIBRATION_CURRENTS = [0, 10, 20, 50, 100, 500, 1000, 1500]
	
		response = raw_input("""
Please choose one of the following calibration options:

V) Calibrate Voltage
C) Calibrate Current
Q) Quit

Enter a menu option: """)
		if response == "V" or response == "v":
			calibrate_voltage(channel)
		elif response == "C" or response == "c":
			calibrate_current(channel)
		elif response == "Q" or response == "q":
			break;



########## Main Menu ##########

def main_menu():
	while True:
		CHANNEL_COUNT = ord(send_generic_message_with_response(dev, MESSAGE_CHANNELS)[1])

		response = raw_input("""
Please choose from the following options:
1 - """  + str(CHANNEL_COUNT) + """) Calibrate a channel
D) Set DAC Addresses
A) Set AREF source (VCC or DAC)
Q) Quit

Enter a menu option: """)
		if response == "D" or response == "d":
			configure_dac_address()
		elif response == "A" or response == "a":
			configure_aref()
		elif response == "Q" or response == "q":
			break
		elif int(response) >= 1 and int(response) <= CHANNEL_COUNT:
			calibrate_menu(int(response) - 1)
	



########## Main startup hooks here ##########

#Constants (mirrored from the C++ code)
MESSAGE_CHANNELS				= 1
MESSAGE_CHANNEL_INFO			= 2
MESSAGE_ACTUAL					= 3
MESSAGE_ACTUAL_RAW				= 4
MESSAGE_SETPOINT				= 5
MESSAGE_SETPOINT_RAW			= 6
MESSAGE_CHANGE_SETPOINT			= 7
MESSAGE_CHANGE_SETPOINT_RAW		= 8
MESSAGE_GET_CALIBRATION			= 9
MESSAGE_SET_CALIBRATION			= 10
MESSAGE_CONFIGURE_DAC_ADDRESS	= 11
MESSAGE_CONFIGURE_AREF			= 12
MESSAGE_BOOTLOADER_JUMP			= 13

TARGET_VOLTAGE					= 0
TARGET_CURRENT					= 1



if (__name__=="__main__"):
	try:
		dev = hid.Device(vid=0x4200, pid=0xFF01)
	
		main_menu()

	finally:
		if ("close" in dir(dev)):
			dev.close()
