import pygame
import serial
import threading
import time
import os
import re
import RPi.GPIO as gpio

###########################
# Drum Slave software, meant to run on the computer (Raspberry Pi in this case).
# When running, we have the concept of Kit, Instrument, and Sample.
# A "Kit" is the type of kit in use: TR-808, Acoustic, Metal, whatever.
# There are one or more "Instruments" in a kit, which represent each physical component
# (e.g. "snare", "bass", "crash", etc.
# Each instrument contains one or more "Samples", which are loaded from .wav files.
#
# See docs/Kit Sample Organization.txt for information on sample folder structure
###########################


# Output data to the LCD.  Call "write(text)" to write data.  Use a newline 
# character to go to the next line of the display.
class Display:
	def __init__(self, pin_rs=19, pin_e=21, pins_db=[22, 23, 24, 26]):

		self.pin_rs=pin_rs
		self.pin_e=pin_e
		self.pins_db=pins_db

		gpio.setmode(gpio.BOARD)
		gpio.setwarnings(False)
		gpio.setup(self.pin_e, gpio.OUT)
		gpio.setup(self.pin_rs, gpio.OUT)
		for pin in self.pins_db:
			gpio.setup(pin, gpio.OUT)

		self.clear()

	def clear(self):
		""" Blank / Reset LCD """

		self.__cmd(0x28) # 4 bit mode, 2 lines, 5x8
		self.__cmd(0x28) # 4 bit mode, 2 lines, 5x8 (the second time)
		self.__cmd(0x0C) # Display on, cursor off, cursor blink off
		self.__cmd(0x01) # Clear display

	def __cmd(self, bits, char_mode=False):
		""" Send command to LCD """

		time.sleep(0.002)
		bits=bin(bits)[2:].zfill(8)

		gpio.output(self.pin_rs, char_mode)

		for i in range(4):
			if bits[i] == "1":
				gpio.output(self.pins_db[::-1][i], True)
			else:
				gpio.output(self.pins_db[::-1][i], False)

		gpio.output(self.pin_e, True)
		gpio.output(self.pin_e, False)

		for i in range(4,8):
			if bits[i] == "1":
				gpio.output(self.pins_db[::-1][i-4], True)
			else:
				gpio.output(self.pins_db[::-1][i-4], False)

		gpio.output(self.pin_e, True)
		gpio.output(self.pin_e, False)

	def write(self, text, position=0, clear=True):
		"""
		Send string to LCD. Newline wraps to second line, after blanking the remainder of the current line.
		Use 'position' argument to write to a particular position in DRAM (keeping in mind that the first
		line starts at position 0x00, and the second line starts at 0x40.
		"""
		
		if clear:
			self.clear()
		
		self.__cmd(0x80 + position)
		
		for char in text:
			if char == '\n':
				for i in range(16):
					self.__cmd(ord(' '), True)
				self.__cmd(0xC0) # next line
			else:
				self.__cmd(ord(char),True)

# A Kit is the entire drum set.  All access to instruments (play, mute, etc) should go through 
# the kit's API.
class Kit:
	def __init__(self):
		Kit.samplesFolder = "samples"

	def lookupInstrument(self, channel):
		return ["snare", "bass", "tom1", "tom2", "tom3", "drum_x", "hihat", "hihat_pedal", "crash", "splash", "ride", "cymbal_x", "ride", "splash", "crash", "hihat"][channel]

	def getName(self):
		return self.kitName

	def loadSamples(self, number):
		self.number = number
		self.instruments = {}
		
		number = "{0:0=3d}".format(number)
		kitPath = os.path.join(Kit.samplesFolder, number)
		with open(os.path.join(kitPath, 'kit.txt'), 'r') as f:
			self.kitName = f.readline()
		
		print("Loading Samples for Kit " + self.kitName + " (" + number + ")")
		for instrument in ["snare", "bass", "tom1", "tom2", "tom3", "drum_x", "hihat", "crash", "splash", "ride", "cymbal_x"]:
			if instrument == "hihat":
				self.instruments[instrument] = HiHat(kitPath)
			else:
				self.instruments[instrument] = Instrument(instrument, kitPath)
			
	def play(self, instrument, volume=1.0):
		if instrument in self.instruments:
			self.instruments[instrument].play(volume)

	def pedal(self, instrument, position=0.0):
		if instrument in self.instruments:
			self.instruments[instrument].pedal(position)

	def mute(self, instrument):
		if instrument in self.instruments:
			self.instruments[instrument].mute()


# An instrument is a single piece of the drum set; for instance, the snare, 
# the bass, a crash cymbal, etc.  There can be one or more samples associated 
# with a single instrument.  These samples must be named "01.wav", "02.wav", etc.
# A given sample is chosen from a linear mapping to requested volume.
class Instrument:
	def __init__(self, name, kitPath):
		self.name = name
		self.samples = []
		
		instrumentPath = os.path.join(kitPath, name)
		if os.path.isdir(instrumentPath):
			print(" Loading Samples for Instrument '" + name + "'")
			samples = [sample for sample in os.listdir(instrumentPath) if re.compile("[0-9][0-9]\\.wav").match(sample)]
			samples.sort()
		
			for sample in samples:
				print("  " + sample)
				self.samples.append(pygame.mixer.Sound(os.path.join(instrumentPath, sample)))
		else:
			print(" No Samples for Instrument '" + name + "'")
	
	def getChannel(self, volume):
		channel = pygame.mixer.find_channel(True)
		if channel.get_busy():
			print("Warning: no free channels")
			channel.stop()
		channel.set_volume(volume)
		return channel
		
	def getIndex(self, value, length):
		index = int((-1 * value * length) + length)
		if index >= length:
			return length - 1
		return index
	
	def play(self, volume):
		channel = self.getChannel(volume)
		
		sample_index = self.getIndex(volume, len(self.samples))
		channel.play(self.samples[sample_index])
		
	def mute(self):
		for sample in self.samples:
			sample.fadeout(500)
			
# A hihat is a special instrument which contains multiple sample mappings for different event types.
class HiHat(Instrument):
	def __init__(self, kitPath):
		HiHat.recent_positions_count = 10
	
		self.name = "hihat"
		self.samples = {}		#A dictionary of name / sample array pairs
		self.available_types = []		#All hihat types that have at least one sample in them
		self.recent_positions = [0]		# Here we store the last X positions; used for splash sounds
		
		for hihat_type in ["tight", "closed", "loose", "open", "chic", "splash"]:
			self.samples[hihat_type] = []

			instrumentPath = os.path.join(kitPath, "hihat_" + hihat_type)
			if os.path.isdir(instrumentPath):
				print(" Loading Samples for Instrument 'hihat_" + hihat_type + "'")
				
				samples = [sample for sample in os.listdir(instrumentPath) if re.compile("[0-9][0-9]\\.wav").match(sample)]
				samples.sort()

				if len(samples) > 0 and hihat_type != "chic" and hihat_type != "splash":
					self.available_types.append(hihat_type)
					
				for sample in samples:
					print("  " + sample)
					self.samples[hihat_type].append(pygame.mixer.Sound(os.path.join(instrumentPath, sample)))
			else:
				print(" No Samples for Instrument 'hihat_" + hihat_type + "'")


	def play(self, volume):
		if len(self.available_types) == 0:
			return
		
		channel = self.getChannel(volume)

		sample_type_index = self.getIndex(self.recent_positions[-1], len(self.available_types))

		print(self.available_types)
		print(sample_type_index)
		sample_type = self.available_types[sample_type_index]

		sample_index = self.getIndex(volume, len(self.samples[sample_type]))
		channel.play(self.samples[sample_type][sample_index])
	
	def pedal(self, position):
		if position > 0.8 and min(self.recent_positions) < 0.2:
			value = position - min(self.recent_positions)
			print("Playing hihat_chic")
			if len(self.samples["chic"]) > 0:
				channel = self.getChannel(value)
				sample_index = self.getIndex(value, len(self.samples["chic"]))
				channel.play(self.samples["chic"][sample_index])
			
		elif position < 0.2 and min(self.recent_positions) < 0.2 and max(self.recent_positions) > 0.8:
			value = max(self.recent_positions) - min(self.recent_positions)
			print("Playing hihat_splash")
			if len(self.samples["splash"]) > 0:
				channel = self.getChannel(value)
				sample_index = self.getIndex(value, len(self.samples["splash"]))
				channel.play(self.samples["splash"][sample_index])

		self.recent_positions.append(position)
		if (len(self.recent_positions) > HiHat.recent_positions_count):
			self.recent_positions.pop(0)
	
	def mute(self):
		for sample_type in self.samples:
			for sample in self.samples[sample_type]:
				sample.fadeout(500)

class SerialMonitor:
	def __init__(self, callback):
		def listener():
			START = 0x7e
			ESCAPE = 0x7d
			MAX_SIZE = 40

			err = False
			esc = False
			position = 0
			length = 0
			command = 0
			checksum = 0x00

			message = {}
			
			ser = serial.Serial('/dev/ttyAMA0', 115200)
			while True:
				b = ord(ser.read())
				if (err and b == START):
					# recover from error condition
					print("Recover from error condition")
					err = False
					position = 0
				elif (err):
					continue

				if (position > 0 and b == START):
					# unexpected start of frame
					print("Unexpected start of frame")
					err = True
					continue

				if (position > 0 and b == ESCAPE):
					# unescape next byte
					esc = True
					continue
				
				if (esc):
					# unescape current byte
					b = 0x20 ^ b
					esc = False

				if (position > 1):
					checksum = (checksum + b) & 0xFF
				
				if (position == 0):
					position = position + 1
					continue
				elif (position == 1):
					length = b
					position = position + 1
					continue
				elif (position == 2):
					command = b
					position = position + 1
					continue
				else:
					if (position > MAX_SIZE):
						# this probably can't happen
						print("Position > MAX_SIZE")
						continue

					if (position == (length + 2)):
						if (checksum == 0xff):
							#Finished the message; pass it to the callback
							callback(command, message)
							
						else:
							err = True;
							print("Invalid checksum")
						position = 0;
						checksum = 0;
					else:
						message[position - 3] = b
						position = position + 1
		
		self.serialThread = threading.Thread(target=listener)
		self.serialThread.daemon = True
		self.serialThread.start()

class ButtonMonitor:
	def __init__(self, callback):
		gpio.setmode(gpio.BOARD)
		buttons = [11, 12, 13, 15]
		for button in buttons:
			gpio.setup(button, gpio.IN)

		def listener():
			state = [False, False, False, False]
			while True:
				time.sleep(0.01)
				for i in range(len(buttons)):
					newState = gpio.input(buttons[i])
					if newState != state[i]:
						state[i] = newState
						if newState == True:
							callback(i)
							
		self.buttonThread = threading.Thread(target=listener)
		self.buttonThread.daemon = True
		self.buttonThread.start()

######################################################################################

kit = Kit()
display = Display()


def handleSerialMessage(command, message):
	if command & 0xF0 == 0x00:
		channel = command
		print("Playing channel " + str(channel))
		if (channel < 12):
			kit.play(kit.lookupInstrument(channel), volume = message[1] / 255.0)
		else:
			kit.mute(kit.lookupInstrument(channel))
			
	elif command == 0x10:
		print("Nack for command " + str(message[0]))
	
	elif command == 0x11:
		print("Ack for command " + str(message[0]))
		
def handleButtonPress(button):
	if not(hasattr(handleButtonPress, 'state')):
		handleButtonPress.state = {"menuId": 0, "shiftId": 0}
	
	state = handleButtonPress.state
	
	if button == 0x00:
		state["menuId"] = state["menuId"] + 1
		state["shiftId"] = 0
	elif button == 0x01:
		state["shiftId"] = state["shiftId"] + 1
		
	if state["menuId"] > 3:
		state["menuId"] = 0
		
	mode = state["menuId"]
	
	if mode == 0:
		showKit()
		
	elif mode == 1:
		showVolume()
		
	elif mode == 2:
		showCalibration()
		
	elif mode == 3:
		showShutdown()

def showSplash():
	display.write("Drum Master\nRev 2.0")
	
def showKit():
	display.write(kit.getName() + "\n")
	
def showVolume():
	display.write("Master")
	
def showCalibration():
	display.write("Calibration")
	
def showShutdown():
	display.write("Shutdown\n(Shift=Off)")
	
	

########## Main startup hooks here ##########
if (__name__=="__main__"):

	pygame.mixer.init(frequency=44100, size=-16, channels=2, buffer=1024)
	pygame.mixer.set_num_channels(16)

	showSplash()
	kit.loadSamples(0)
	showKit()
	
	sm = SerialMonitor(handleSerialMessage)
	bm = ButtonMonitor(handleButtonPress)

	
	
	'''
	for x in range(0, 4):

		kit.pedal("hihat", position = 0.0)
		kit.play("hihat")
		time.sleep(0.5)
		kit.pedal("hihat", position = 0.2)
		kit.play("hihat")
		time.sleep(0.5)
		kit.pedal("hihat", position = 0.4)
		kit.play("hihat")
		time.sleep(0.5)
		kit.pedal("hihat", position = 0.6)
		kit.play("hihat")
		time.sleep(0.5)
		kit.pedal("hihat", position = 0.8)
		kit.play("hihat")
		time.sleep(0.5)
		kit.pedal("hihat", position = 1.0)
		kit.play("hihat")
		time.sleep(0.5)
		kit.pedal("hihat", position = 0.0)
		time.sleep(0.1)
		kit.pedal("hihat", position = 1.0)
		time.sleep(1.0)

		kit.play("ride")
		time.sleep(0.3)
		kit.play("ride", volume=0.0)
		time.sleep(0.15)
		kit.play("ride", volume=0.5)
		time.sleep(0.15)
		kit.play("ride")
		kit.play("snare", volume=0.48)
		time.sleep(0.3)
		kit.play("ride", volume=0.5)
		time.sleep(0.15)
		kit.play("ride", volume=0.5)
		time.sleep(0.15)
		kit.play("ride")
		time.sleep(0.3)
		kit.play("ride", volume=0.5)
		time.sleep(0.15)
		kit.play("ride", volume=0.5)
		time.sleep(0.15)
		kit.play("ride")
		kit.play("snare")
		kit.mute("ride")
		time.sleep(0.6)
	'''
	
	while(True):
		time.sleep(10)
		print("tick")
	
	'''
	while pygame.mixer.get_busy():
		time.sleep(0.1)
	'''