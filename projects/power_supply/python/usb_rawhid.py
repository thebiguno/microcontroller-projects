# Sample program demonstrating USB Raw HID access using the pyhidapi library
# (a ctypes wrapper around hidapi).  Sends alternating arrays containing
# 0xFF and 0x00, and prints the byte values of any recieved data.
#
# Designed to interface with usb_rawhid sample on the atmega32u2
#############################################################################
import hid, traceback, time

print(hid.enumerate())

dev = hid.Device(vid=0x4200, pid=0xFF01)
try:
	dev.nonblocking = 0
	print(dev.manufacturer)
	print(dev.product)
	print(dev.serial)
	b = '\x00'

	while True:
		arr = dev.read(6)
		voltage = (ord(arr[2]) << 8) + ord(arr[3]);
		current = (ord(arr[4]) << 8) + ord(arr[5]);
		print("Read channel " + str(ord(arr[1])) + ", voltage=" + str(voltage) + ", current=" + str(current) + "\n")
except Exception, e:
	traceback.print_exc()
dev.close()
