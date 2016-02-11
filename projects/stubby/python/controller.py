import sys
import pygame
import serial
import time
import os 

#Center the pygame window on screen
os.environ['SDL_VIDEO_CENTERED'] = '1'

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
	
	print(data)
	for b in data:
		print(hex(ord(b)))
		ser.write(b)

def keyboard_listener():
	keys = {}
	
	while True:
		for event in pygame.event.get():
			# determine if X was clicked, or Ctrl+W or Alt+F4 was used 
			if event.type == pygame.QUIT:
				print("Quitting...")
				return
				
			if event.type == pygame.KEYDOWN or event.type == pygame.KEYUP:
				if event.key in [pygame.K_t, pygame.K_RCTRL, pygame.K_LCTRL]:
					if event.key == pygame.K_t:
						button_id = CONTROLLER_BUTTON_VALUE_START
					elif event.key == pygame.K_RCTRL:
						button_id = CONTROLLER_BUTTON_VALUE_RIGHT2
					elif event.key == pygame.K_LCTRL:
						button_id = CONTROLLER_BUTTON_VALUE_LEFT2
					
					if event.type == pygame.KEYDOWN:
						write(MESSAGE_UC_BUTTON_PUSH, [button_id])
					else:
						write(MESSAGE_UC_BUTTON_RELEASE, [button_id])
					
					
				if event.key in [pygame.K_LEFT, pygame.K_RIGHT, pygame.K_UP, pygame.K_DOWN, pygame.K_a, pygame.K_s, pygame.K_d, pygame.K_w]:
					if event.type == pygame.KEYDOWN:
						keys[event.key] = True
					else:
						del keys[event.key]
				
					if pygame.K_a in keys:
						left_x = 0
					elif pygame.K_d in keys:
						left_x = 255
					else:
						left_x = 128

					if pygame.K_w in keys:
						left_y = 0
					elif pygame.K_s in keys:
						left_y = 255
					else:
						left_y = 128
						
					if pygame.K_LEFT in keys:
						right_x = 0
					elif pygame.K_RIGHT in keys:
						right_x = 255
					else:
						right_x = 128

					if pygame.K_UP in keys:
						right_y = 0
					elif pygame.K_DOWN in keys:
						right_y = 255
					else:
						right_y = 128
					
					write(MESSAGE_ANNOUNCE_CONTROL_ID, [ord('U')])
				
					analog = [left_x, left_y, right_x, right_y]
					write(MESSAGE_UC_JOYSTICK_MOVE, analog)
					print(keys)
					
			
				

########## Main startup hooks here ##########
if (__name__=="__main__"):
	if (len(sys.argv) != 2):
		print("Usage: " + sys.argv[0] + " <port>")
		sys.exit(0)

	ser = serial.Serial(sys.argv[1], 38400, timeout=1)
	
	pygame.init()
	pygame.display.set_mode([320, 200])
	pygame.key.set_repeat(1, 500) #No delay, 500ms interval
	
	write(MESSAGE_ANNOUNCE_CONTROL_ID, [ord('U')])
	
	keyboard_listener()
