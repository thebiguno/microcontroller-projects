import pygame
import serial
import threading
import time
import os
import re

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

# A Kit is the entire drum set.  All access to instruments (play, mute, etc) should go through 
# the kit's API.
class Kit:
	def __init__(self):
		Kit.samplesFolder = "samples"

	def lookupInstrument(self, channel):
		return ["snare", "bass", "tom1", "tom2", "tom3", "drum_x", "hihat", "hihat_pedal", "crash", "splash", "ride", "cymbal_x", "ride", "splash", "crash", "hihat"][channel]

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
			#ser = serial.Serial('/dev/ttyAMA0', 115200, timeout=1)
			from random import randint
			message = [0, 0]
			while True:
				#b = ord(ser.read())
				time.sleep(randint(0,10) / 100.0)
				b = randint(0,255)
				if (message[0] == 0x00 and (b & 0xC0 == 0xC0)):
					message[0] = b
					print("0 = " + str(b))
				elif (message[0] & 0xC0 == 0xC0):
					message[1] = b
					if self.verifyChecksum(message) == 0:
						callback(list(message))
					else:
						print("Invalid checksum")
					message[0] = 0
					message[1] = 0
				else:
					print("Invalid packet")
					message[0] = 0x00
					message[1] = 0x00
				
		
		self.serialThread = threading.Thread(target=listener)
		self.serialThread.daemon = True
		self.serialThread.start()
	
	def verifyChecksum(self, message):
		checksum = 0
		
		#TODO Is there a more pythonic way to do this?
		checksum = checksum ^ ((message[0] >> 6) & 0x03)
		checksum = checksum ^ ((message[0] >> 4) & 0x03)
		checksum = checksum ^ ((message[0] >> 2) & 0x03)
		checksum = checksum ^ ((message[0] >> 0) & 0x03)

		checksum = checksum ^ ((message[1] >> 6) & 0x03)
		checksum = checksum ^ ((message[1] >> 4) & 0x03)
		checksum = checksum ^ ((message[1] >> 2) & 0x03)
		checksum = checksum ^ ((message[1] >> 0) & 0x03)
		
		return checksum

########## Main startup hooks here ##########
if (__name__=="__main__"):

	pygame.mixer.init(frequency=44100, size=-16, channels=2, buffer=1024)
	pygame.mixer.set_num_channels(16)

	kit = Kit()
	kit.loadSamples(0)
'''
	def callback(message):
		print(str(message))
		channel = ((message[0] >> 2) & 0x0F)

		if (channel < 12):
			kit.play(kit.lookupInstrument(channel), volume = message[1] / 255.0)
		else:
			kit.mute(kit.lookupInstrument(channel))
	
	sm = SerialMonitor(callback)
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
