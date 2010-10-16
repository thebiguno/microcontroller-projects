#!/opt/local/bin/python2.6
#
# Reads from a serial port sending binary data.
#
###################

import serial

ser = serial.Serial("/dev/tty.usbserial-A200294u", 57600)

while True:
	b0 = ord(ser.read())
	if (b0 & 0xF0 == 0xF0):
		b1 = ord(ser.read())
		b2 = ord(ser.read())

		channel = ((b0 & 0x0F) << 2) | (b1 >> 6)
		velocity = ((b1 & 0x3F) << 4) | (b2 >> 4)

		checksum = (b0 >> 4) ^ (b0 & 0x0F) ^ (b1 >> 4) ^ (b1 & 0x0F) ^ (b2 >> 4) ^ (b2 & 0x0F)

		if (checksum == 0):
			print(hex(b0) + " " + hex(b1) + " " + hex(b2))
			print(str(channel) + ":" + str(velocity))
		else:
			print("Checksum error: " + str(checksum))

	else:
		print("Invalid sequence read.")
