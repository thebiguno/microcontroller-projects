import pygame
import serial
import Queue as queue
import threading
import traceback
import time
import os
import sys
import signal
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
		
		cgram = [
			0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,			# 0 (Use space)
			0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1f,			# 1
			0x0,0x0,0x0,0x0,0x0,0x0,0x1f,0x1f,			# 2
			0x0,0x0,0x0,0x0,0x0,0x1f,0x1f,0x1f,			# 3
			0x0,0x0,0x0,0x0,0x1f,0x1f,0x1f,0x1f,		# 4
			0x0,0x0,0x0,0x1f,0x1f,0x1f,0x1f,0x1f,		# 5
			0x0,0x0,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,		# 6
			0x0,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f		# 7
														# 8 (Use 0xFF)
		]
		self.__cmd(0x40)	#Go to beginning of CGRAM section
		for b in cgram:
			self.__cmd(b, True)


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
		Kit.samples_folder = "samples"
		Kit.named_channels = ["snare", "bass", "tom1", "tom2", "tom3", "drum_x", "hihat", "hihat_pedal", "crash", "splash", "ride", "cymbal_x", "ride", "splash", "crash", "hihat"]
		Kit.named_instruments = ["snare", "bass", "tom1", "tom2", "tom3", "drum_x", "hihat", "crash", "splash", "ride", "cymbal_x"]
		
		self.number = None
		self.kit_name = None
		self.instruments = {}
		self.volume = 1.0

	def get_named_channel(self, channel):
		return Kit.named_channels[channel]
	
	def get_instrument(self, instrument_number):
		return Kit.named_instruments[instrument_number]
	
	def get_instrument_number(self, instrument):
		return Kit.named_instruments.index(instrument)
	
	def get_name(self):
		return self.kit_name
	
	@property
	def next_number(self):
		dirs = [d for d in os.listdir(Kit.samples_folder) if re.compile("[0-9][0-9][0-9]").match(d)]
		dirs.sort()
		try:
			index = dirs.index(self.number_formatted)
			index = index + 1
		except ValueError:
			index = 0;
		if index >= len(dirs):
			index = 0
		return int(dirs[index])
	
	@property
	def number_formatted(self):
		return "{0:0=3d}".format(self.number)
	
	@property
	def volumes(self):
		volumes = {}
		for instrument_name in Kit.named_instruments:
			if instrument_name in self.instruments:
				volumes[instrument_name] = self.instruments[instrument_name].volume
			else:
				volumes[instrument_name] = 1
		return volumes
	
	@volumes.setter
	def volumes(self, volumes):
		if "master" in volumes:
			self.volume = volumes["master"]
			if self.volume < 0:
				self.volume = 0
			elif self.volume > 1:
				self.volume = 1
		else:
			self.volume = 1
		
		
		for instrument in self.instruments:
			if instrument.name in volumes:
				instrument.volume = volume
			else:
				instrument.volume = 1
				
	
	@property
	def kit_path(self):
		return os.path.join(Kit.samples_folder, self.number_formatted)

	def load_samples(self, number):
		self.number = number
		self.instruments = {}
		
		kit_path = self.kit_path
		try:
			with open(os.path.join(kit_path, 'kit.txt'), 'r') as f:
				self.kit_name = f.readline()
		except IOError:
			self.load_samples(self.next_number)
			return
		
		print("Loading Samples for Kit " + self.kit_name + " (" + self.number_formatted + ")")
		for instrument in Kit.named_instruments:
			if instrument == "hihat":
				self.instruments[instrument] = HiHat(self)
			else:
				self.instruments[instrument] = Instrument(instrument, self)
	
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
	def __init__(self, name, kit):
		self.name = name
		self.kit = kit
		self.samples = []
		self._volume = 1.0
		
		kit_path = self.kit.kit_path
		
		instrument_path = os.path.join(kit_path, name)
		if os.path.isdir(instrument_path):
			print(" Loading Samples for Instrument '" + name + "'")
			samples = [sample for sample in os.listdir(instrument_path) if re.compile("[0-9][0-9]\\.wav").match(sample)]
			samples.sort()
		
			for sample in samples:
				print("  " + sample)
				self.samples.append(pygame.mixer.Sound(os.path.join(instrument_path, sample)))
		else:
			print(" No Samples for Instrument '" + name + "'")
	
	def get_channel(self, volume):
		channel = pygame.mixer.find_channel(True)
		if channel.get_busy():
			print("Warning: no free channels")
			channel.stop()
		channel.set_volume(volume * self.volume * self.kit.volume)
		return channel
	
	def get_index(self, value, length):
		index = int((-1 * value * length) + length)
		if index >= length:
			return length - 1
		return index
	
	@property
	def volume(self):
		return self._volume
	
	@volume.setter
	def volume(self, volume):
		if volume < 0:
			volume = 0
		elif volume > 1:
			volume = 1
		self.volume = volume
		
	def play(self, volume):
		channel = self.get_channel(volume)
		
		sample_index = self.get_index(volume, len(self.samples))
		channel.play(self.samples[sample_index])
		
	def mute(self):
		for sample in self.samples:
			sample.fadeout(500)
			
# A hihat is a special instrument which contains multiple sample mappings for different event types.
class HiHat(Instrument):
	def __init__(self, kit):
		HiHat.recent_positions_count = 10
	
		self.name = "hihat"
		self.kit = kit
		self.samples = {}		#A dictionary of name / sample array pairs
		self.available_types = []		#All hihat types that have at least one sample in them
		self.recent_positions = [0]		# Here we store the last X positions; used for splash sounds
		self._volume = 1.0
		
		kit_path = self.kit.kit_path
		
		for hihat_type in ["tight", "closed", "loose", "open", "chic", "splash"]:
			self.samples[hihat_type] = []

			instrument_path = os.path.join(kit_path, "hihat_" + hihat_type)
			if os.path.isdir(instrument_path):
				print(" Loading Samples for Instrument 'hihat_" + hihat_type + "'")
				
				samples = [sample for sample in os.listdir(instrument_path) if re.compile("[0-9][0-9]\\.wav").match(sample)]
				samples.sort()

				if len(samples) > 0 and hihat_type != "chic" and hihat_type != "splash":
					self.available_types.append(hihat_type)
					
				for sample in samples:
					print("  " + sample)
					self.samples[hihat_type].append(pygame.mixer.Sound(os.path.join(instrument_path, sample)))
			else:
				print(" No Samples for Instrument 'hihat_" + hihat_type + "'")


	def play(self, volume):
		if len(self.available_types) == 0:
			return
		
		channel = self.get_channel(volume)

		sample_type_index = self.get_index(self.recent_positions[-1], len(self.available_types))

		print(self.available_types)
		print(sample_type_index)
		sample_type = self.available_types[sample_type_index]

		sample_index = self.get_index(volume, len(self.samples[sample_type]))
		channel.play(self.samples[sample_type][sample_index])
	
	def pedal(self, position):
		if position > 0.8 and min(self.recent_positions) < 0.2:
			value = position - min(self.recent_positions)
			print("Playing hihat_chic")
			if len(self.samples["chic"]) > 0:
				channel = self.get_channel(value)
				sample_index = self.get_index(value, len(self.samples["chic"]))
				channel.play(self.samples["chic"][sample_index])
			
		elif position < 0.2 and min(self.recent_positions) < 0.2 and max(self.recent_positions) > 0.8:
			value = max(self.recent_positions) - min(self.recent_positions)
			print("Playing hihat_splash")
			if len(self.samples["splash"]) > 0:
				channel = self.get_channel(value)
				sample_index = self.get_index(value, len(self.samples["splash"]))
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
		SerialMonitor.START = 0x7e
		SerialMonitor.ESCAPE = 0x7d
		SerialMonitor.MAX_SIZE = 40
		
		self.lock = threading.Lock()

		def read_listener():
			err = False
			esc = False
			position = 0
			length = 0
			command = 0
			checksum = 0x00

			message = [None] * SerialMonitor.MAX_SIZE
			
			self.serial = serial.Serial('/dev/ttyAMA0', 38400)
			while True:
				try:
					b = ord(self.serial.read())
					if (err and b == SerialMonitor.START):
						# recover from error condition
						print("Recover from error condition")
						err = False
						position = 0
					elif (err):
						continue

					if (position > 0 and b == SerialMonitor.START):
						# unexpected start of frame
						print("Unexpected start of frame")
						err = True
						continue

					if (position > 0 and b == SerialMonitor.ESCAPE):
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
						print("Received length: " + hex(length))
						position = position + 1
						continue
					elif (position == 2):
						command = b
						print("Received command: " + hex(command))
						position = position + 1
						continue
					else:
						if (position > SerialMonitor.MAX_SIZE):
							# this probably can't happen
							print("Position > MAX_SIZE")
							continue

						if (position == (length + 2)):
							if (checksum == 0xff):
								print("Checksum verified")
								#Finished the message; pass it to the callback
								callback(command, message[:(position - 3)])
								
							else:
								err = True;
								print("Invalid checksum")
							position = 0;
							checksum = 0;
						else:
							message[position - 3] = b
							position = position + 1
							print("Received message byte " + hex(b))
				except Exception:
					print("Unhandled exception in read_listener; continuing")
					traceback.print_exc()
		
		self.read_thread = threading.Thread(name="SerialReadListener", target=read_listener)
		self.read_thread.daemon = True
		self.read_thread.start()
		
		self.write_queue = queue.Queue()
		
		def write_listener():
			while True:
				try:
					b = self.write_queue.get()
					print("Writing byte " + hex(ord(b)))
					self.serial.write(str(b))
				except Exception:
					print("Unhandled exception in write_listener; continuing")
					traceback.print_exc()
				
		self.write_thread = threading.Thread(name="SerialWriteListener", target=write_listener)
		self.write_thread.daemon = True
		self.write_thread.start()
	
	def write(self, command, message):
		'''
		Write the given command + message to the serial port (via a blocking queue to handle thread safety).  The command 
		MUST be a single unsigned byte between 0 and 255.  The message MUST be a list of numbers between 0x00 and 0xFF.
		'''
		def append_byte(data, b, escape=True):
			if (escape and (b == SerialMonitor.START or b == SerialMonitor.ESCAPE)):
				data.append(chr(SerialMonitor.ESCAPE))
				data.append(chr(b ^ 0x20))
			else:
				data.append(chr(b))

		data = []
		append_byte(data, SerialMonitor.START, escape=False)
		append_byte(data, len(message) + 1)
		append_byte(data, command)
		
		checksum = command
		for b in message:
			checksum = (checksum + b) & 0xFF
			append_byte(data, b)
			
		append_byte(data, (0xFF - checksum))
		
		with self.lock:	#We don't want multiple threads pushing data into the queue at the same time
			for b in data:
				self.write_queue.put(b)

class ButtonMonitor:
	def __init__(self, callback):
		buttons = [11, 12, 13, 15]
		for button in buttons:
			gpio.setup(button, gpio.IN)

		def button_listener():
			state = [False, False, False, False]
			while True:
				try:
					time.sleep(0.01)
					for i in range(len(buttons)):
						newState = gpio.input(buttons[i])
						if newState != state[i]:
							state[i] = newState
							if newState == True:
								callback(i)
				except Exception:
					print("Unhandled exception in button_listener; continuing")
					traceback.print_exc()
							
		self.button_thread = threading.Thread(name="ButtonMonitor", target=button_listener)
		self.button_thread.daemon = True
		self.button_thread.start()

######################################################################################

# Global variables

kit = Kit()
display = Display()
serial_monitor = None
button_monitor = None

def gpio_init():
	gpio.setmode(gpio.BOARD)
	gpio.setwarnings(False)

	gpio.setup(7, gpio.OUT)
	gpio.output(7, True)

def handle_serial_message(command, message):
	if command & 0xF0 == 0x00:
		channel = command
		print("Playing channel " + str(channel))
		print(kit.get_named_channel(channel))
		print(message)
		if (channel < 12):
			kit.play(kit.get_named_channel(channel), volume = message[0] / 255.0)
		else:
			kit.mute(kit.get_named_channel(channel))
			
	elif command == 0x10:
		print("Nack for command " + str(message[0]))
	
	elif command == 0x11:
		print("Ack for command " + str(message[0]))
	
	elif command == 0x31:
		print("Received 'load volume' command")
	
	elif command == 0x33:
		print("Received 'load kit' command: " + str(message[0]))
		kit.load_samples(message[0])
		show_kit()

		
def handle_button_press(button):
	if not(hasattr(handle_button_press, 'state')):
		handle_button_press.state = {"menuId": 0, "shiftId": 0}
	
	state = handle_button_press.state
	
	if button == 0x00:
		state["menuId"] = state["menuId"] + 1
		state["shiftId"] = 0
		
	if state["menuId"] > 3:
		state["menuId"] = 0
	
	print(handle_button_press.state)
	mode = state["menuId"]
	
	if mode == 0:
		if button == 0x01:
			#Save the current volume state to EEPROM
			volumes = kit.volumes
			message = [None] * 8
			message[0] = kit.number & 0x7F
			message[1] = int(kit.volume * 255) & 0xFF
			message[2] = (int(kit.instruments["snare"].volume * 16) & 0x0F) << 4 + (int(kit.instruments["drum_x"].volume * 16) & 0x0F)
			message[3] = (int(kit.instruments["bass"].volume * 16) & 0x0F) << 4 + (int(kit.instruments["tom1"].volume * 16) & 0x0F)
			message[4] = (int(kit.instruments["tom2"].volume * 16) & 0x0F) << 4 + (int(kit.instruments["tom3"].volume * 16) & 0x0F)
			message[5] = (int(kit.instruments["hihat"].volume * 16) & 0x0F) << 4 + (int(kit.instruments["ride"].volume * 16) & 0x0F)
			message[6] = (int(kit.instruments["crash"].volume * 16) & 0x0F) << 4 + (int(kit.instruments["splash"].volume * 16) & 0x0F)
			message[7] = (int(kit.instruments["cymbal_x"].volume * 16) & 0x0F) << 4
			print("Saving volume state: " + str(message))
			serial_monitor.write(0x31, message)

			#Load the next set of samples
			kit.load_samples(kit.next_number)
			
			#Load the new volume states from EEPROM
			serial_monitor.write(0x30, [kit.number])
			
		show_kit()
		
	elif mode == 1:
		show_volume()
		
	elif mode == 2:
		show_calibration()
		
	elif mode == 3:
		if button == 0x01:
			show_shutdown(shutting_down=True)
			serial_monitor.write(0x33, [kit.number])
			#os.system("shutdown -h now")
			time.sleep(1) #Wait for serial write to finish
			gpio.output(7, False)
			os.kill(os.getpid(), signal.SIGINT)	#os.exit kills the local thread only.
		else:
			show_shutdown()

def show_splash():
	display.write("Drum Master\nRev 2.0")
	
def show_kit(refresh=True):
	if refresh:
		display.write(kit.number_formatted + " " + kit.get_name())
def show_volume():
	display.write("Master")
	
def show_calibration():
	display.write("Calibration")
	
def show_shutdown(shutting_down=False):
	if shutting_down:
		display.write("Shutting down...")
	else:
		display.write("Shutdown\n(Shift=Off)")
	

########## Main startup hooks here ##########
if (__name__=="__main__"):

	gpio_init()

	pygame.mixer.init(frequency=44100, size=-16, channels=2, buffer=1024)
	pygame.mixer.set_num_channels(16)

	show_splash()
	
	serial_monitor = SerialMonitor(handle_serial_message)

	# Request last used kit from AVR repeatedly until it replies.
	while kit.number == None:
		serial_monitor.write(0x32, [])
		time.sleep(5)

	show_kit()
	button_monitor = ButtonMonitor(handle_button_press)

	
	
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
		time.sleep(5)
		print("***** Threads:")
		for thread in threading.enumerate():
			print("Thread: " + thread.name + " Daemon: " + str(thread.daemon))
	
	'''
	while pygame.mixer.get_busy():
		time.sleep(0.1)
	'''