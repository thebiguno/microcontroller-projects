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
	
def send_calibration_message_with_response(dev, message, channel, target, value):
	tx_buffer = [chr(message), chr(channel), chr(target)] + list(struct.pack("<f", value))
	#print("Sending Message: " + ''.join(hex(ord(x)) + "," for x in tx_buffer))
	dev.write(''.join(tx_buffer))

	rx_buffer = dev.read(16, 1000)
	if len(rx_buffer) == 0:
		raise Exception("Unable to read data")
	#print("Received Message: " + ''.join(hex(ord(x)) + "," for x in rx_buffer))
	result = [ord(rx_buffer[0]), ord(rx_buffer[1]), ord(rx_buffer[2]), struct.unpack("<f", ''.join(rx_buffer[3:7]))[0]]
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


def calibrate_voltage():
	#Set voltage as low as possible, and current limiting off (let anything through)
	voltage_setpoint = 0
	send_measurement_message_with_response(dev, MESSAGE_CHANGE_SETPOINT_RAW, channel, voltage_setpoint, 0xFFF)
	
#Voltage Actual + Setpoint Low
	print("""Adjust measured voltage to exactly """ + str(VOLTAGE_MEASURED_VALUE_LOW) + """ mV.  Hit enter when target reached.
Enter a DAC value as an integer between 0 and 4095, or change the current DAC value with
an offset adjustment as an integer starting with a + / -""")

	while True:
		response = raw_input("DAC Value for " + str(VOLTAGE_MEASURED_VALUE_LOW) + "mV: ")
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
	#Set voltage as low as possible, and current limiting off (let anything through)
	voltage_setpoint = send_measurement_message_with_response(dev, MESSAGE_CHANGE_SETPOINT, channel, 8000, 0)[2]
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

def configure_dac_address():
	old_address = -1
	new_address = -1
	while old_address < 0 or old_address > 2:
		old_address = int(raw_input("Enter the current I2C address (0-2): "))
	while new_address < 0 or old_address > 2 or new_address == old_address:
		new_address = int(raw_input("Enter the new I2C address (0-2): "))
	raw_input("""Connect PORTB0 (Encoder 1A) to the I2C_PROG header with a jumper wire.
It may be a good idea to watch the procedure with a logic probe to verify correct operation.
Press enter when ready.""")
	send_generic_message_with_response(dev, MESSAGE_CONFIGURE_DAC_ADDRESS, [chr(old_address), chr(new_address)])
	print("The DAC should now have the new address, but you need to verify.")

def configure_aref():
	dac_number = -1
	dac_channel = -1
	while dac_number < 0 or dac_number > 2:
		dac_number = int(raw_input("Enter the DAC number (0-2): "))
	while dac_channel < 0 or dac_channel > 3:
		dac_channel = int(raw_input("Enter the DAC channel (0-3): "))
	raw_input("""Configuring DAC """ + str(dac_number) + """ channel """ + str(dac_channel) + """ as AREF voltage source.
Press enter when ready.""")
	send_generic_message_with_response(dev, MESSAGE_CONFIGURE_AREF, [chr(dac_number), chr(dac_channel)])

try:
	dev = hid.Device(vid=0x4200, pid=0xFF01)
	
	MESSAGE_CHANNELS				= 1
	MESSAGE_ACTUAL					= 2
	MESSAGE_ACTUAL_RAW				= 3
	MESSAGE_SETPOINT				= 4
	MESSAGE_SETPOINT_RAW			= 5
	MESSAGE_CHANGE_SETPOINT			= 6
	MESSAGE_CHANGE_SETPOINT_RAW		= 7
	MESSAGE_GET_CALIBRATION			= 8
	MESSAGE_SET_CALIBRATION			= 9
	MESSAGE_CONFIGURE_DAC_ADDRESS	= 10
	MESSAGE_CONFIGURE_AREF			= 11

	TARGET_VOLTAGE_ACTUAL_SLOPE			= 0
	TARGET_VOLTAGE_ACTUAL_OFFSET		= 1
	TARGET_VOLTAGE_SETPOINT_SLOPE		= 2
	TARGET_VOLTAGE_SETPOINT_OFFSET		= 3
	TARGET_CURRENT_ACTUAL_SLOPE			= 4
	TARGET_CURRENT_ACTUAL_OFFSET		= 5
	TARGET_CURRENT_SETPOINT_SLOPE		= 6
	TARGET_CURRENT_SETPOINT_OFFSET		= 7

	VOLTAGE_MEASURED_VALUE_LOW = 0
	VOLTAGE_MEASURED_VALUE_HIGH = 8000
	CURRENT_MEASURED_VALUE_LOW = 0
	CURRENT_MEASURED_VALUE_HIGH = 800
	
	CHANNEL_COUNT = ord(send_generic_message_with_response(dev, MESSAGE_CHANNELS)[1])

	integer = re.compile('^[0-9]+$')
	increment = re.compile('^\\+[0-9]+$')
	decrement = re.compile('^-[0-9]+$')

	while True:
		channel = int(raw_input("Channel (1 - " + str(CHANNEL_COUNT) + "): "))
		if (channel >= 1 and channel <= CHANNEL_COUNT):
			channel = channel - 1;	#Internally this is zero based.
			break;
		print("Invalid channel.  Please enter a valid channel number.")

	while True:

		print("Current calibration values:")
		print("Actual:   V = (" + str(send_calibration_message_with_response(dev, MESSAGE_GET_CALIBRATION, channel, TARGET_VOLTAGE_ACTUAL_SLOPE, 0)[3]) + " * ADC_VALUE) + " + str(send_calibration_message_with_response(dev, MESSAGE_GET_CALIBRATION, channel, TARGET_VOLTAGE_ACTUAL_OFFSET, 0)[3]))
		print("Setpoint: V = (" + str(send_calibration_message_with_response(dev, MESSAGE_GET_CALIBRATION, channel, TARGET_VOLTAGE_SETPOINT_SLOPE, 0)[3]) + " * DAC_VALUE) + " + str(send_calibration_message_with_response(dev, MESSAGE_GET_CALIBRATION, channel, TARGET_VOLTAGE_SETPOINT_OFFSET, 0)[3]))
		print("Actual:   I = (" + str(send_calibration_message_with_response(dev, MESSAGE_GET_CALIBRATION, channel, TARGET_CURRENT_ACTUAL_SLOPE, 0)[3]) + " * ADC_VALUE) + " + str(send_calibration_message_with_response(dev, MESSAGE_GET_CALIBRATION, channel, TARGET_CURRENT_ACTUAL_OFFSET, 0)[3]))
		print("Setpoint: I = (" + str(send_calibration_message_with_response(dev, MESSAGE_GET_CALIBRATION, channel, TARGET_CURRENT_SETPOINT_SLOPE, 0)[3]) + " * DAC_VALUE) + " + str(send_calibration_message_with_response(dev, MESSAGE_GET_CALIBRATION, channel, TARGET_CURRENT_SETPOINT_OFFSET, 0)[3]))

		response = raw_input(
"""
To calibrate the power supply, you must perform the following steps:
1) Find the linear calibration values for voltage (both actual readings and setpoints)
2) Find the linear calibration values for current readings
3) Find the linear calibration values for current limit setpoints

To do this, you will need a multimeter which has both a voltage and current sensing (up to at least 1A, preferably 10A) mode.

Please choose one of the following calibration options:

V) Calibrate Voltage
C) Calibrate Current
D) Set DAC Address
A) Set AREF source
Q) Quit

Enter a menu option: """)

		if response == "V" or response == "v":
			calibrate_voltage()
		elif response == "C" or response == "c":
			calibrate_current()
		elif response == "D" or response == "d":
			configure_dac_address()
		elif response == "A" or response == "a":
			configure_aref();
		elif response == "Q" or response == "q":
			break;


finally:
	if ("close" in dir(dev)):
		dev.close()
