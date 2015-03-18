#!/usr/bin/python
# Sample program demonstrating USB Raw HID access using the pyhidapi library
# (a ctypes wrapper around hidapi).  Sends alternating arrays containing
# 0xFF and 0x00, and prints the byte values of any recieved data.
#
# Designed to interface with usb_rawhid sample on the atmega32u2
#############################################################################
import hid, re, struct, traceback, time

def send_generic_message_with_response(dev, message, body):
	tx_buffer = [chr(message)]
	if body != None:
		tx_buffer = tx_buffer + body
	if len(tx_buffer) == 1:
		tx_buffer.append(chr(0))
	print("Sending Message: " + ''.join(hex(ord(x)) + "," for x in tx_buffer))
	dev.write(''.join(tx_buffer))

	rx_buffer = dev.read(16, 1000)
	if len(rx_buffer) == 0:
		raise Exception("Unable to read data")
	print("Received Message: " + ''.join(hex(ord(x)) + "," for x in rx_buffer))
	return rx_buffer;
	
def send_calibration_message_with_response(dev, message, channel, target, value):
	tx_buffer = [chr(message), chr(channel), chr(target)] + list(struct.pack("<f", value))
	print("Sending Message: " + ''.join(hex(ord(x)) + "," for x in tx_buffer))
	dev.write(''.join(tx_buffer))

	rx_buffer = dev.read(16, 1000)
	if len(rx_buffer) == 0:
		raise Exception("Unable to read data")
	print("Received Message: " + ''.join(hex(ord(x)) + "," for x in rx_buffer))
	result = [ord(rx_buffer[0]), ord(rx_buffer[1]), ord(rx_buffer[2]), struct.unpack("<f", ''.join(rx_buffer[3:]))[0]]
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
	print("Received Message: " + ''.join(hex(ord(x)) + "," for x in rx_buffer))
	result = [ord(rx_buffer[0])]
	if len(rx_buffer) > 1:
		result.append(ord(rx_buffer[1]))
	if len(rx_buffer) > 3:
		result.append(ord(rx_buffer[2]) * 256 + ord(rx_buffer[3]))
	if len(rx_buffer) > 5:
		result.append(ord(rx_buffer[4]) * 256 + ord(rx_buffer[5]))
	return result;

try:
	dev = hid.Device(vid=0x4200, pid=0xFF01)
	
	MESSAGE_CHANNELS				= 1
	MESSAGE_ACTUAL					= 2
	MESSAGE_ACTUAL_RAW				= 3
	MESSAGE_SETPOINT				= 4
	MESSAGE_SETPOINT_RAW			= 5
	MESSAGE_CHANGE_SETPOINT			= 6
	MESSAGE_CHANGE_SETPOINT_RAW		= 7

	TARGET_VOLTAGE_ACTUAL_SLOPE			= 0
	TARGET_VOLTAGE_ACTUAL_OFFSET		= 1
	TARGET_VOLTAGE_SETPOINT_SLOPE		= 2
	TARGET_VOLTAGE_SETPOINT_OFFSET		= 3
	TARGET_CURRENT_ACTUAL_SLOPE			= 4
	TARGET_CURRENT_ACTUAL_OFFSET		= 5
	TARGET_CURRENT_SETPOINT_SLOPE		= 6
	TARGET_CURRENT_SETPOINT_OFFSET		= 7

	VOLTAGE_MEASURED_VALUE_LOW = 0
	VOLTAGE_MEASURED_VALUE_LOW = 8
	
	CHANNEL_COUNT = send_generic_message_with_response(dev, MESSAGE_CHANNELS)[1]

	integer = re.compile('^[0-9]+$')
	increment = re.compile('^\\+[0-9]+$')
	decrement = re.compile('^-[0-9]+$')

	while True:
		channel = int(raw_input("Channel (0 - " + str(CHANNEL_COUNT - 1) + "): "))
		if (channel >= 0 and channel < CHANNEL_COUNT):
			break;
		print("Invalid channel.  Please enter a valid channel number.")

	
	print(
"""
To calibrate the power supply, you must perform the following steps:
1) Find the linear calibration values for voltage (both actual readings and setpoints)
2) Find the linear calibration values for current readings
3) Find the linear calibration values for current limit setpoints

To do this, you will need a multimeter which has both a voltage and current sensing (up to at least 1A, preferably 10A) mode.

Please follow the instructions on screen to continue calibration.
""")

	rx_buffer = send_measurement_message_with_response(dev, MESSAGE_SETPOINT_RAW, channel)
	voltage_setpoint = rx_buffer[2]
	current_setpoint = rx_buffer[3]
	
	while True:
		response = raw_input(
"""
Adjust measured voltage to exactly """ + str(VOLTAGE_MEASURED_VALUE_LOW) + """"V.  Hit enter when complete.
Enter an absolute value as an integer between 0 and 4095, or an offset adjustment as an integer starting with a + / -:
Value: """)
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

		voltage_setpoint = send_measurement_message_with_response(dev, MESSAGE_CHANGE_SETPOINT_RAW, channel, voltage_setpoint, current_setpoint)[2] & 0x0FFF
		print("Voltage setpoint: " + str(voltage_setpoint))
		
	raw_voltage_actual_low = send_measurement_message_with_response(dev, MESSAGE_ACTUAL_RAW, channel)[2] & 0x03FF

	while True:
		response = raw_input(
"""
Adjust measured voltage to exactly """ + str(VOLTAGE_MEASURED_VALUE_HIGH) + """"V.  Hit enter when complete.
Enter an absolute value as an integer between 0 and 4095, or an offset adjustment as an integer starting with a + / -:
Value: """)
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

		voltage_setpoint = send_measurement_message_with_response(dev, MESSAGE_CHANGE_SETPOINT_RAW, channel, voltage_setpoint, current_setpoint)[2] & 0x0FFF
		print("Voltage setpoint: " + str(voltage_setpoint))

	raw_voltage_actual_high = send_measurement_message_with_response(dev, MESSAGE_ACTUAL_RAW, channel)[2] & 0x03FF
	
#TODO adjust current readings and current limit setpoint.

	print(raw_voltage_setpoint_low)
	print(raw_voltage_actual_low)
	print(raw_voltage_setpoint_high)
	print(raw_voltage_actual_high)
	
	
	#We need to find the calibration values to translate the raw values into actual voltages.  To do this, we plot
	# the values on an X/Y plane, with raw values on the X axis and voltages on the Y axis.  We know the actual values
	# (as measured by a multimeter), and we have the raw values, so we can find slope and offset.
	#First is to find slope: slope is delta Y / delta X.
	#Then we find the offset:  offset is found using y - y1 = m(x - x1).  We know a point (x1,y1), and we need to solve for y when x is 0.
	
	voltage_actual_slope = (VOLTAGE_MEASURED_VALUE_HIGH - VOLTAGE_MEASURED_VALUE_LOW) / (raw_voltage_actual_high - raw_voltage_atual_low)
	voltage_actual_offset = voltage_actual_slope(0 - raw_voltage_actual_high) + VOLTAGE_MEASURED_VALUE_HIGH
	voltage_setpoint_slope = (VOLTAGE_MEASURED_VALUE_HIGH - VOLTAGE_MEASURED_VALUE_LOW) / (raw_voltage_setpoint_high - raw_voltage_setpoint_low)
	voltage_setpoint_offset = voltage_setpoint_slope(0 - raw_voltage_setpoint_high) + VOLTAGE_MEASURED_VALUE_HIGH
		
	while True:
		print("Actual:   V = (" + str(voltage_actual_slope) + " * ADC_VALUE) + " + str(voltage_actual_offset))
		print("Setpoint: V = (" + str(voltage_setpoint_slope) + " * DAC_VALUE) + " + str(voltage_setpoint_offset))

		response = raw_input(
"""
Do you want to save these calibration values to EEPROM?
Y/N: """)
		if response == "Y" or response == "y":
			print("Saved values:")
			print("Actual:   V = (" + str(send_calibration_message_with_response(dev, message, channel, TARGET_VOLTAGE_ACTUAL_SLOPE, voltage_actual_slope)[3]) + " * ADC_VALUE) + " + str(send_calibration_message_with_response(dev, message, channel, TARGET_VOLTAGE_ACTUAL_OFFSET, voltage_actual_offset))
			print("Setpoint: V = (" + str(send_calibration_message_with_response(dev, message, channel, TARGET_VOLTAGE_SETPOINT_SLOPE, voltage_setpoint_slope)[3]) + " * DAC_VALUE) + " + str(send_calibration_message_with_response(dev, message, channel, TARGET_VOLTAGE_SETPOINT_OFFSET, voltage_setpoint_offset))

			send_calibration_message_with_response(dev, message, channel, TARGET_VOLTAGE_ACTUAL_SLOPE, voltage_actual_slope)

finally:
	if ("close" in dir(dev)):
		dev.close()
