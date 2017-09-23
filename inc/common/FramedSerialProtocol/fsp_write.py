# This is meant to be either imported as a module (import fsp_write) or
# the functions copied into the target program.

def __escape_byte(message, b):
	if (b == 0x7e or b == 0x7d):
		message.append(0x7d);
		message.append(b ^ 0x20);
	else:
		message.append(b);

def write(command, data):
	message = [0x7e]
	__escape_byte(message, len(data) + 1)
	__escape_byte(message, command)
	checksum = command
	for b in data:
		b = ord(b)
		__escape_byte(message, b)
		checksum = (checksum + b) & 0xFF
	__escape_byte(message, (0xFF - checksum) % 0xFF)
	return ''.join(chr(b) for b in message)		#This is a string of raw byte values which can be written to the serial port
