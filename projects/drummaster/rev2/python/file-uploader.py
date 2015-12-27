#!/usr/bin/env python2.6
#
# Reads from a serial port sending ascii data.
#
###################

import serial, sys, os, time

#Special bytes
BYTE_START = "\x7e"
BYTE_ESCAPE = "\x7d"
BYTE_SEPARATOR = "\x7c"


ser = serial.Serial(sys.argv[1])

mappingsFileSelected = False
for i, filename in enumerate(sys.argv):
	if (os.path.basename(filename) == "MAPPINGS.TXT"):
		mappingsFileSelected = True
		break

if (mappingsFileSelected == False):
	print("You must include a mappings file (MAPPINGS.TXT) in the upload selection")
	sys.exit()

print("Uploading " + str(len(sys.argv) - 2) + " files...")
for i, filename in enumerate(sys.argv):
	if i >= 2:
		startTime = time.time();
		sys.stdout.write(str(i - 1) + ": ")
		sys.stdout.write(filename)
		sys.stdout.flush()
		

		f = open(filename, "rb")
		fileLength = os.path.getsize(filename)
		try:
			encoded = []
			#Start byte
			encoded.append(BYTE_START)
			#Filename
			for byte in os.path.basename(filename):
				encoded.append(byte)
			#End of filename
			encoded.append(BYTE_SEPARATOR)
			
			#File length (uint32_t)
			encoded.append(chr((fileLength >> 24) & 0xFF));
			encoded.append(chr((fileLength >> 16) & 0xFF));
			encoded.append(chr((fileLength >> 8) & 0xFF));
			encoded.append(chr((fileLength >> 0) & 0xFF));
			encoded.append(BYTE_SEPARATOR)
			
			#Binary data, with escaping
			for byte in f.read():
				if byte == BYTE_START or byte == BYTE_ESCAPE:
					encoded.append(BYTE_ESCAPE)
					encoded.append(chr(ord(byte) ^ 0x20))
				else:
					encoded.append(byte);
			
			#Write end of data byte
			encoded.append(BYTE_START)
			ser.write("".join(encoded))
		 
		finally:
			f.close()
			
		endTime = time.time();
		print(" (" + str(round(fileLength / 1024 / (endTime - startTime), 2)) + " KB/s)");

print("All files uploaded")