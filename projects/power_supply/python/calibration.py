#!/usr/bin/python
# Sample program demonstrating USB Raw HID access using the pyhidapi library
# (a ctypes wrapper around hidapi).  Sends alternating arrays containing
# 0xFF and 0x00, and prints the byte values of any recieved data.
#
# Designed to interface with usb_rawhid sample on the atmega32u2
#############################################################################
import hid, re, traceback, time

def send(dev, message, channel=None, voltage=None, current=None):
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

	CHANNEL_COUNT = send(dev, MESSAGE_CHANNELS)[1]

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

	rx_buffer = send(dev, MESSAGE_SETPOINT_RAW, channel)
	voltage_setpoint = rx_buffer[2]
	current_setpoint = rx_buffer[3]
	
	while True:
		response = raw_input(
"""
Adjust measured voltage to exactly 0.000V.  Hit enter when complete.
Enter an absolute value as an integer between 0 and 4095, or an offset adjustment as an integer starting with a + / -:
Value: """)
		if increment.match(response):
			voltage_setpoint = (voltage_setpoint + int(response[1:]))
			
		elif decrement.match(response):
			voltage_setpoint = (voltage_setpoint - int(response[1:]))
			
		elif integer.match(response):
			voltage_setpoint = int(response) & 0x0FFF;

		elif response == "":
			raw_voltage_low = voltage_setpoint
			break;
		
		else:
			print("Invalid option.")

		voltage_setpoint = send(dev, MESSAGE_CHANGE_SETPOINT_RAW, channel, voltage_setpoint, current_setpoint)[2] & 0x0FFF
		print("Voltage setpoint: " + str(voltage_setpoint))

	while True:
		response = raw_input(
"""
Adjust measured voltage to exactly 8.000V.  Hit enter when complete.
Enter an absolute value as an integer between 0 and 4095, or an offset adjustment as an integer starting with a + / -:
Value: """)
		if increment.match(response):
			voltage_setpoint = (voltage_setpoint + int(response[1:]))
			
		elif decrement.match(response):
			voltage_setpoint = (voltage_setpoint - int(response[1:]))
			
		elif integer.match(response):
			voltage_setpoint = int(response) & 0x0FFF;

		elif response == "":
			raw_voltage_high = voltage_setpoint
			break;
		
		else:
			print("Invalid option.")

		voltage_setpoint = send(dev, MESSAGE_CHANGE_SETPOINT_RAW, channel, voltage_setpoint, current_setpoint)[2] & 0x0FFF
		print("Voltage setpoint: " + str(voltage_setpoint))

		
#TODO adjust current readings and current limit setpoint.

	print(raw_voltage_low)
	print(raw_voltage_high)

finally:
	if ("close" in dir(dev)):
		dev.close()
