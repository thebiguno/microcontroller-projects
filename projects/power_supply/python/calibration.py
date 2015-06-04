#!/usr/bin/python
# Sample program demonstrating USB Raw HID access using the pyhidapi library
# (a ctypes wrapper around hidapi).  Sends alternating arrays containing
# 0xFF and 0x00, and prints the byte values of any recieved data.
#
# Designed to interface with usb_rawhid sample on the atmega32u2
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
	return rx_buffer;
	
def send_calibration_message_with_response(dev, message, channel, target, index, dac, adc, measured):
	tx_buffer = [chr(message), chr(channel), chr(target), chr(index)] + list(struct.pack("<H", dac)) + list(struct.pack("<H", adc)) + list(struct.pack("<h", measured))
	#print("Sending Message: " + ''.join(hex(ord(x)) + "," for x in tx_buffer))
	dev.write(''.join(tx_buffer))

	rx_buffer = dev.read(16, 1000)
	if len(rx_buffer) == 0:
		raise Exception("Unable to read data")
	#print("Received Message: " + ''.join(hex(ord(x)) + "," for x in rx_buffer))
	result = [ord(rx_buffer[0]), ord(rx_buffer[1]), ord(rx_buffer[2], ord(rx_buffer[3]), struct.unpack("<H", ''.join(rx_buffer[4:5]))[0], struct.unpack("<H", ''.join(rx_buffer[6:7]))[0], struct.unpack("<h", ''.join(rx_buffer[8:9]))[0]]
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
	#print("Sending Message: " + ''.join(hex(ord(x)) + "," for x in tx_buffer))
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
	#Set voltage as low as possible, and current limiting off (let anything through)
	voltage_setpoint = 0
	send_measurement_message_with_response(dev, MESSAGE_CHANGE_SETPOINT_RAW, channel, voltage_setpoint, 0xFFF)
	
	for calibration_voltage in calibration_voltages:
		print("""Adjust measured voltage to exactly """ + str(calibration_voltage) + """ mV.  Hit enter when target reached.
	Enter a DAC value as an integer between 0 and 4095""")

		while True:
			response = raw_input("DAC Value for " + str(calibration_voltage) + "mV: ")
			if increment.match(response):
				voltage_setpoint = (voltage_setpoint + int(response[1:]))
			
			elif decrement.match(response):
				voltage_setpoint = (voltage_setpoint - int(response[1:]))
			
			elif integer.match(response):
				voltage_setpoint = int(response) & 0x0FFF;

			elif response == "":
				raw_voltage_setpoint_low = voltage_setpoint
				break;
		
			else:
				print("Invalid option.")

			voltage_setpoint = send_measurement_message_with_response(dev, MESSAGE_CHANGE_SETPOINT_RAW, channel, voltage_setpoint, 0xFFF)[2] & 0x0FFF
			print("Voltage setpoint: " + str(voltage_setpoint))
		
		raw_voltage_actual_low = send_measurement_message_with_response(dev, MESSAGE_ACTUAL_RAW, channel)[2] & 0x03FF

#Voltage Actual + Setpoint High
	print("""Adjust measured voltage to exactly """ + str(VOLTAGE_MEASURED_VALUE_HIGH) + """ mV.  Hit enter when target reached.
Enter a DAC value as an integer between 0 and 4095, or change the current DAC value with
an offset adjustment as an integer starting with a + / -""")
	while True:
		response = raw_input("DAC Value for " + str(VOLTAGE_MEASURED_VALUE_HIGH) + "mV: ")
		if increment.match(response):
			voltage_setpoint = (voltage_setpoint + int(response[1:]))
			
		elif decrement.match(response):
			voltage_setpoint = (voltage_setpoint - int(response[1:]))
			
		elif integer.match(response):
			voltage_setpoint = int(response) & 0x0FFF;

		elif response == "":
			raw_voltage_setpoint_high = voltage_setpoint
			break;
		
		else:
			print("Invalid option.")

		voltage_setpoint = send_measurement_message_with_response(dev, MESSAGE_CHANGE_SETPOINT_RAW, channel, voltage_setpoint, 0xFFF)[2] & 0x0FFF
		print("Voltage setpoint: " + str(voltage_setpoint))

	raw_voltage_actual_high = send_measurement_message_with_response(dev, MESSAGE_ACTUAL_RAW, channel)[2] & 0x03FF

	#We need to find the calibration values to translate the raw values into actual voltages.  To do this, we plot
	# the values on an X/Y plane, with raw values on the X axis and voltages on the Y axis.  We know the actual values
	# (as measured by a multimeter), and we have the raw values, so we can find slope and offset.
	#First is to find slope: slope is delta Y / delta X.
	#Then we find the offset:  offset is found using y - y1 = m(x - x1).  We know a point (x1,y1), and we need to solve for y when x is 0.
	
	voltage_actual_slope = float(VOLTAGE_MEASURED_VALUE_HIGH - VOLTAGE_MEASURED_VALUE_LOW) / (raw_voltage_actual_high - raw_voltage_actual_low)
	voltage_actual_offset = voltage_actual_slope * (0 - raw_voltage_actual_high) + VOLTAGE_MEASURED_VALUE_HIGH
	voltage_setpoint_slope = float(VOLTAGE_MEASURED_VALUE_HIGH - VOLTAGE_MEASURED_VALUE_LOW) / (raw_voltage_setpoint_high - raw_voltage_setpoint_low)
	voltage_setpoint_offset = voltage_setpoint_slope * (0 - raw_voltage_setpoint_high) + VOLTAGE_MEASURED_VALUE_HIGH

	while True:
		print("Voltage Actual:   V = (" + str(voltage_actual_slope) + " * ADC_VALUE) + " + str(voltage_actual_offset))
		print("Voltage Setpoint: V = (" + str(voltage_setpoint_slope) + " * DAC_VALUE) + " + str(voltage_setpoint_offset))

		response = raw_input(
"""
Do you want to save these calibration values to EEPROM?
Y/N: """)
		if response == "Y" or response == "y":
			print("Saved values:")
			print("Voltage Actual:   V = (" + str(send_calibration_message_with_response(dev, MESSAGE_SET_CALIBRATION, channel, TARGET_VOLTAGE_ACTUAL_SLOPE, voltage_actual_slope)[3]) + " * ADC_VALUE) + " + str(send_calibration_message_with_response(dev, MESSAGE_SET_CALIBRATION, channel, TARGET_VOLTAGE_ACTUAL_OFFSET, voltage_actual_offset)[3]))
			print("Voltage Setpoint: V = (" + str(send_calibration_message_with_response(dev, MESSAGE_SET_CALIBRATION, channel, TARGET_VOLTAGE_SETPOINT_SLOPE, voltage_setpoint_slope)[3]) + " * DAC_VALUE) + " + str(send_calibration_message_with_response(dev, MESSAGE_SET_CALIBRATION, channel, TARGET_VOLTAGE_SETPOINT_OFFSET, voltage_setpoint_offset)[3]))
			break;
		elif response == "N" or response == "n":
			print("Calibration discarded.")
			break;

def calibrate_current():
	#Set voltage to the high value, and set the current setpoint to 0 (let nothing through)
	voltage_setpoint = send_measurement_message_with_response(dev, MESSAGE_CHANGE_SETPOINT, channel, VOLTAGE_MEASURED_VALUE_HIGH, 0)[2]
	current_setpoint = 0
	send_measurement_message_with_response(dev, MESSAGE_CHANGE_SETPOINT_RAW, channel, voltage_setpoint, current_setpoint)
	
#Current Actual + Setpoint Low
	print("""Plug in a multimeter in current mode between Vout and GND.  Adjust measured current to exactly """ + str(CURRENT_MEASURED_VALUE_LOW) + """ mV.  Hit enter when target reached.
Enter a DAC value as an integer between 0 and 4095, or change the current DAC value with
an offset adjustment as an integer starting with a + / -""")

	while True:
		response = raw_input("DAC Value for " + str(CURRENT_MEASURED_VALUE_LOW) + "mA: ")
		if increment.match(response):
			current_setpoint = (current_setpoint + int(response[1:]))
			
		elif decrement.match(response):
			current_setpoint = (current_setpoint - int(response[1:]))
			
		elif integer.match(response):
			current_setpoint = int(response) & 0x0FFF;

		elif response == "":
			raw_current_setpoint_low = current_setpoint
			break;
		
		else:
			print("Invalid option.")

		current_setpoint = send_measurement_message_with_response(dev, MESSAGE_CHANGE_SETPOINT_RAW, channel, voltage_setpoint, current_setpoint)[3] & 0x0FFF
		print("Current setpoint: " + str(current_setpoint))
		
	raw_current_actual_low = send_measurement_message_with_response(dev, MESSAGE_ACTUAL_RAW, channel)[3] & 0x03FF

#Current Actual + Setpoint High
	print("""Adjust measured voltage to exactly """ + str(CURRENT_MEASURED_VALUE_HIGH) + """ mA.  Hit enter when target reached.
Enter a DAC value as an integer between 0 and 4095, or change the current DAC value with
an offset adjustment as an integer starting with a + / -""")
	while True:
		response = raw_input("DAC Value for " + str(CURRENT_MEASURED_VALUE_HIGH) + "mA: ")
		if increment.match(response):
			current_setpoint = (current_setpoint + int(response[1:]))
			
		elif decrement.match(response):
			current_setpoint = (current_setpoint - int(response[1:]))
			
		elif integer.match(response):
			current_setpoint = int(response) & 0x0FFF;

		elif response == "":
			raw_current_setpoint_high = current_setpoint
			break;
		
		else:
			print("Invalid option.")

		current_setpoint = send_measurement_message_with_response(dev, MESSAGE_CHANGE_SETPOINT_RAW, channel, voltage_setpoint, current_setpoint)[3] & 0x0FFF
		print("Current setpoint: " + str(current_setpoint))

	raw_current_actual_high = send_measurement_message_with_response(dev, MESSAGE_ACTUAL_RAW, channel)[3] & 0x03FF

	#We need to find the calibration values to translate the raw values into actual voltages.  To do this, we plot
	# the values on an X/Y plane, with raw values on the X axis and voltages on the Y axis.  We know the actual values
	# (as measured by a multimeter), and we have the raw values, so we can find slope and offset.
	#First is to find slope: slope is delta Y / delta X.
	#Then we find the offset:  offset is found using y - y1 = m(x - x1).  We know a point (x1,y1), and we need to solve for y when x is 0.
	
	current_actual_slope = float(CURRENT_MEASURED_VALUE_HIGH - CURRENT_MEASURED_VALUE_LOW) / (raw_current_actual_high - raw_current_actual_low)
	current_actual_offset = current_actual_slope * (0 - raw_current_actual_high) + CURRENT_MEASURED_VALUE_HIGH
	current_setpoint_slope = float(CURRENT_MEASURED_VALUE_HIGH - CURRENT_MEASURED_VALUE_LOW) / (raw_current_setpoint_high - raw_current_setpoint_low)
	current_setpoint_offset = current_setpoint_slope * (0 - raw_current_setpoint_high) + CURRENT_MEASURED_VALUE_HIGH

	while True:
		print("Current Actual:   A = (" + str(current_actual_slope) + " * ADC_VALUE) + " + str(current_actual_offset))
		print("Current Setpoint: A = (" + str(current_setpoint_slope) + " * DAC_VALUE) + " + str(current_setpoint_offset))

		response = raw_input(
"""
Do you want to save these calibration values to EEPROM?
Y/N: """)
		if response == "Y" or response == "y":
			print("Saved values:")
			print("Current Actual:   A = (" + str(send_calibration_message_with_response(dev, MESSAGE_SET_CALIBRATION, channel, TARGET_CURRENT_ACTUAL_SLOPE, current_actual_slope)[3]) + " * ADC_VALUE) + " + str(send_calibration_message_with_response(dev, MESSAGE_SET_CALIBRATION, channel, TARGET_CURRENT_ACTUAL_OFFSET, current_actual_offset)[3]))
			print("Current Setpoint: A = (" + str(send_calibration_message_with_response(dev, MESSAGE_SET_CALIBRATION, channel, TARGET_CURRENT_SETPOINT_SLOPE, current_setpoint_slope)[3]) + " * DAC_VALUE) + " + str(send_calibration_message_with_response(dev, MESSAGE_SET_CALIBRATION, channel, TARGET_CURRENT_SETPOINT_OFFSET, current_setpoint_offset)[3]))
			break;
		elif response == "N" or response == "n":
			print("Calibration discarded.")
			break;

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
		elif response == "Q" or response == "q"
			break;

		response = raw_input("""
0-2) Choose the new I2C address
Q) Quit

Enter a menu option: """)
		if int(response) >= 0 and int(response) <= 2:
			new_address = int(response)
		elif response == "Q" or response == "q"
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
		channel_info = send_generic_message_with_response(dev, MESSAGE_CHANNEL_INFO)
		
		CALIBRATION_VOLTAGES = [0, 1800, 2500, 3300, 5000, 8000, 10000, 12500]
		CALIBRATION_CURRENTS = [0, 10, 20, 50, 100, 500, 1000, 1500]
	
		voltage_limit = (channel_info[1] << 8) & 
		if (0 != (channel_info[1] << 8) & (1 << channel))):
		

		
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
			calibrate_menu(int(response))
	



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
