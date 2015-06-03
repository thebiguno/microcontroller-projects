#!/usr/bin/python
# Sample program demonstrating USB Raw HID access using the pyhidapi library
# (a ctypes wrapper around hidapi).  Sends alternating arrays containing
# 0xFF and 0x00, and prints the byte values of any recieved data.
#
# Designed to interface with usb_rawhid sample on the atmega32u2
#############################################################################
import hid, time

try:
	dev = hid.Device(vid=0x4200, pid=0xFF01)
	
	MESSAGE_BOOTLOADER_JUMP			= 12

	tx_buffer = [chr(MESSAGE_BOOTLOADER_JUMP)]
	tx_buffer.append(chr(0))
	dev.write(''.join(tx_buffer))
	
	time.sleep(0.1)
	
finally:
	if ("close" in dir(dev)):
		dev.close()
