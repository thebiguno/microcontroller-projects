import os, serial, time

ser = None

MESSAGE_ANNOUNCE_CONTROL_ID			=	0x00

MESSAGE_UC_BUTTON_PUSH				=	0x10
MESSAGE_UC_BUTTON_RELEASE			=	0x11
MESSAGE_UC_BUTTON_NONE				=	0x12
MESSAGE_UC_JOYSTICK_MOVE			=	0x13

CONTROLLER_BUTTON_VALUE_SELECT	=	0x00
CONTROLLER_BUTTON_VALUE_LEFT3	=	0x01
CONTROLLER_BUTTON_VALUE_RIGHT3	=	0x02
CONTROLLER_BUTTON_VALUE_START	=	0x03
CONTROLLER_BUTTON_VALUE_PADUP	=	0x04
CONTROLLER_BUTTON_VALUE_PADRIGHT=	0x05
CONTROLLER_BUTTON_VALUE_PADDOWN	=	0x06
CONTROLLER_BUTTON_VALUE_PADLEFT	=	0x07
CONTROLLER_BUTTON_VALUE_LEFT2	=	0x08
CONTROLLER_BUTTON_VALUE_RIGHT2	=	0x09
CONTROLLER_BUTTON_VALUE_LEFT1	=	0x0A
CONTROLLER_BUTTON_VALUE_RIGHT1	=	0x0B
CONTROLLER_BUTTON_VALUE_TRIANGLE=	0x0C
CONTROLLER_BUTTON_VALUE_CIRCLE	=	0x0D
CONTROLLER_BUTTON_VALUE_CROSS	=	0x0E
CONTROLLER_BUTTON_VALUE_SQUARE	=	0x0F

# Windows
if os.name == 'nt':
	import msvcrt

# Posix (Linux, OS X)
else:
	import sys
	import termios
	import atexit
	from select import select

def write(command, message):
	'''
	Write the given command + message to the serial port.  The command MUST be a single unsigned byte between 0 and 255.  
	The message MUST be a list of numbers between 0x00 and 0xFF.
	'''
	START = 0x7e
	ESCAPE = 0x7d
	
	def append_byte(data, b, escape=True):
		if (escape and (b == chr(START) or b == chr(ESCAPE))):
			data.append(chr(ESCAPE))
			data.append(chr(b ^ 0x20))
		else:
			data.append(chr(b))

	data = [chr(START)]
	append_byte(data, len(message) + 1)
	append_byte(data, command)
	
	checksum = command
	for b in message:
		checksum = (checksum + b) & 0xFF
		data.append(chr(b))
		
	append_byte(data, (0xFF - checksum))
	
	print("Sending command " + hex(command))
	for b in data:
		#print(hex(ord(b)))
		ser.write(b)

class KBHit:

	def __init__(self):
		'''Creates a KBHit object that you can call to do various keyboard things.'''

		if os.name == 'nt':
			pass

		else:

			# Save the terminal settings
			self.fd = sys.stdin.fileno()
			self.new_term = termios.tcgetattr(self.fd)
			self.old_term = termios.tcgetattr(self.fd)

			# New terminal setting unbuffered
			self.new_term[3] = (self.new_term[3] & ~termios.ICANON & ~termios.ECHO)
			termios.tcsetattr(self.fd, termios.TCSAFLUSH, self.new_term)

			# Support normal-terminal reset at exit
			atexit.register(self.set_normal_term)


	def set_normal_term(self):
		''' Resets to normal terminal.  On Windows this is a no-op.
		'''

		if os.name == 'nt':
			pass

		else:
			termios.tcsetattr(self.fd, termios.TCSAFLUSH, self.old_term)


	def getch(self):
		''' Returns a keyboard character after kbhit() has been called.
			Should not be called in the same program as getarrow().
		'''

		s = ''

		if os.name == 'nt':
			return msvcrt.getch().decode('utf-8')

		else:
			return sys.stdin.read(1)


	def getarrow(self):
		''' Returns an arrow-key code after kbhit() has been called. Codes are
		0 : up
		1 : right
		2 : down
		3 : left
		Should not be called in the same program as getch().
		'''

		if os.name == 'nt':
			msvcrt.getch() # skip 0xE0
			c = msvcrt.getch()
			vals = [72, 77, 80, 75]

		else:
			c = sys.stdin.read(3)[2]
			vals = [65, 67, 66, 68]

		return vals.index(ord(c.decode('utf-8')))


	def kbhit(self):
		''' Returns True if keyboard character was hit, False otherwise.
		'''
		if os.name == 'nt':
			return msvcrt.kbhit()

		else:
			dr,dw,de = select([sys.stdin], [], [], 0)
			return dr != []


if (__name__=="__main__"):
	if (len(sys.argv) != 2):
		print("Usage: " + sys.argv[0] + " <port>")
		sys.exit(0)

	ser = serial.Serial(sys.argv[1], 38400, timeout=1)
	
	write(MESSAGE_ANNOUNCE_CONTROL_ID, [ord('U')])
	
	lastJoystick = time.time();
	
	kb = KBHit()

	while(True):
		joyLX = 128
		joyLY = 128
		joyRX = 128
		joyRY = 128
		
		if (kb.kbhit()):
			key = kb.getch()

			if (key == 'a'):
				joyLX = 0
			elif (key == 'd'):
				joyLX = 255

			if (key == 'w'):
				joyLY = 0
			elif (key == 's'):
				joyLY = 255
			
			write(MESSAGE_UC_JOYSTICK_MOVE, [joyRX, joyRY, joyLX, joyLY])
			lastJoystick = time.time();
			
		if (time.time() - lastJoystick > 0.5):
			write(MESSAGE_UC_JOYSTICK_MOVE, [joyRX, joyRY, joyLX, joyLY])
			lastJoystick = time.time();

	kb.set_normal_term()
