# Sample program demonstrating USB Raw HID access using the pyhidapi library
# (a ctypes wrapper around hidapi).  Sends alternating arrays containing
# 0xFF and 0x00, and prints the byte values of any recieved data.
#
# Designed to interface with usb_rawhid sample on the atmega32u2
#############################################################################
import hid, traceback, time

print(hid.enumerate())

dev = hid.Device(vid=5824, pid=1152)
try:
	dev.nonblocking = 0
	print(dev.manufacturer)
	print(dev.product)
	print(dev.serial)
	b = '\x00'

	while True:
		print("Write: " + str(dev.write(b * 64)))
		if b == '\xFF':
			b = '\x00'
		else:
			b = '\xFF'
		arr = dev.read(64)
		print("Read: " + str(len(arr)))
		print(arr + "\n")
		time.sleep(1)
except Exception, e:
	traceback.print_exc()
dev.close()
