#!/usr/bin/python
# Sample program demonstrating USB Raw HID access using the pyhidapi library
# (a ctypes wrapper around hidapi).  Sends alternating arrays containing
# 0xFF and 0x00, and prints the byte values of any recieved data.
#
# Designed to interface with usb_rawhid sample on the atmega32u2
#############################################################################
import hid, traceback, time

digit = re.compile('^[0-9]$')
integer = re.compile('^-?[0-9]+$')

def send(dev, message, body):
	tx_buffer = [message]
	if type(body) is list:
		tx_buffer = tx_buffer + body
	dev.write(''.join(tx_buffer))
	rx_buffer = dev.read(2, 1000)
	if len(rx_buffer) == 0:
		throw 'Unable to read data'
	return rx_buffer;

try:
	dev = hid.Device(vid=0x4200, pid=0xFF01)
	MESSAGE_CHANNELS				= 0
	MESSAGE_ACTUAL					= 1
	MESSAGE_ACTUAL_RAW				= 2
	MESSAGE_SETPOINT				= 3
	MESSAGE_SETPOINT_RAW			= 4
	MESSAGE_CHANGE_SETPOINT			= 5
	MESSAGE_CHANGE_SETPOINT_RAW		= 6

	CHANNEL_COUNT = send(dev, MESSAGE_CHANNELS)[1]

	while True:
		channel = int(raw_input("Channel (0 - " + CHANNEL_COUNT + "): "))
		if (channel >= 0 and channel < CHANNEL_COUNT):
			break;
		print("Invalid channel.  Please enter a valid channel number."

	
	print("""
To calibrate the power supply, we must perform the following steps:
1) Find the linear calibration values for voltage (both actual readings and setpoints)
2) Find the linear calibration values for current readings
3) Find the linear calibration values for current limit setpoints

To do this, you will need a multimeter which has both a voltage and current sensing (up to at least 1A, preferably 10A) mode.

Please follow the instructions on screen to continue calibration.
	""")

	rx_buffer = send(dev, MESSAGE_CHANNELS, [channel])
	voltage_setpoint = rx_buffer[2] << 8 + rx_buffer[3]
	current_setpoint = rx_buffer[4] << 8 + rx_buffer[5]
	
	while True:
		response = raw_input("""
Adjust the voltage to read exactly 0.000V on the multimeter:
Use + and - to increase or decrease the raw DAC value by 1.
Enter a number (12 bit unsigned decimal integer)to set the raw DAC to the specified value.
Hit enter when the output is exactly 0.000V.
		""")
		
		if response = "+":
			voltage_setpoint = (voltage_setpoint + 1) & 0x0FFF
			send(dev, MESSAGE_CHANGE_SETPOINT_RAW, [channel, voltage_setpoint >> 8, voltage_setpoint & 0xFF, current_setpoint >> 8, current_setpoint & 0xFF])
			
		elif response = "-":
			voltage_setpoint = (voltage_setpoint - 1) & 0x0FFF
			send(dev, MESSAGE_CHANGE_SETPOINT_RAW, [channel, voltage_setpoint >> 8, voltage_setpoint & 0xFF, current_setpoint >> 8, current_setpoint & 0xFF])
			
		elif response = "":
			raw_voltage_low = voltage_setpoint
			break;
		
		elif integer.match(response):
			voltage_setpoint = int(response) & 0x0FFF;
			send(dev, MESSAGE_CHANGE_SETPOINT_RAW, [channel, voltage_setpoint >> 8, voltage_setpoint & 0xFF, current_setpoint >> 8, current_setpoint & 0xFF])
		
		else:
			print("Invalid option.")

	while True:
		response = raw_input("""
Adjust the voltage to read exactly 8.000V on the multimeter:
Use + and - to increase or decrease the raw DAC value by 1.
Enter a number (12 bit unsigned decimal integer)to set the raw DAC to the specified value.
Hit enter when the output is exactly 8.000V.
		""")
		
		if response = "+":
			voltage_setpoint = (voltage_setpoint + 1) & 0x0FFF
			send(dev, MESSAGE_CHANGE_SETPOINT_RAW, [channel, voltage_setpoint >> 8, voltage_setpoint & 0xFF, current_setpoint >> 8, current_setpoint & 0xFF])
			
		elif response = "-":
			voltage_setpoint = (voltage_setpoint - 1) & 0x0FFF
			send(dev, MESSAGE_CHANGE_SETPOINT_RAW, [channel, voltage_setpoint >> 8, voltage_setpoint & 0xFF, current_setpoint >> 8, current_setpoint & 0xFF])
			
		elif response = "":
			raw_voltage_high = voltage_setpoint
			break;
		
		elif integer.match(response):
			voltage_setpoint = int(response) & 0x0FFF;
			send(dev, MESSAGE_CHANGE_SETPOINT_RAW, [channel, voltage_setpoint >> 8, voltage_setpoint & 0xFF, current_setpoint >> 8, current_setpoint & 0xFF])
		
		else:
			print("Invalid option.")
		
#TODO adjust current readings and current limit setpoint.

	print(raw_voltage_low)
	print(raw_voltage_high)

finally:
	if ("close" in dir(dev)):
		dev.close()
