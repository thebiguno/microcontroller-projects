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
# The samples folder is configured as follows:
# samples/
#   kit1/
#     snare/
#       01.wav
#       02.wav
#     bass/
#       01.wav
#   kit2/
#     snare/
#       01.wav
#     crash/
#       01.wav
#       02.wav
###########################

# A Kit is the entire drum set.  All access to instruments (play, mute, etc) should go through 
# the kit's API.
class Kit:
	def __init__(self):
		self.samplesFolder = "samples"
		self.channels = ["snare", "snare_alt", "bass", "tom1", "tom2", "tom3", "hihat", "hihat_pedal", "crash", "splash", "ride", "ride_alt", "hihat", "crash", "splash", "ride"]
		
	def getAvailableKits(self):
		instruments = os.listdir(kitPath)
		instruments.sort()
		return instruments
	
	def lookupChannel(self, channel):
		return self.channels[channel]

	def loadSamples(self, name):
		self.name = name
		self.instruments = {}
		
		print("Loading Kit " + name)
		kitPath = os.path.join(self.samplesFolder, name)
		instruments = os.listdir(kitPath)
		instruments.sort()
		for instrument in instruments:
			if os.path.isdir(os.path.join(kitPath, instrument)):
				print(" Loading Instrument " + instrument)
				self.instruments[instrument] = Instrument(instrument, kitPath)
			
	def play(self, instrument, volume=1.0):
		if instrument in self.instruments:
			self.instruments[instrument].play(volume)
		
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
		samples = [sample for sample in os.listdir(instrumentPath) if re.compile("[0-9][0-9]\\.wav").match(sample)]
		samples.sort()
		
		for sample in samples:
			print("  Loading Sample " + sample)
			self.samples.append(pygame.mixer.Sound(os.path.join(instrumentPath, sample)))
			
	def play(self, volume):
		channel = pygame.mixer.find_channel(True)
		if channel.get_busy():
			print("Warning: no free channels")
			channel.stop()
		channel.set_volume(volume)

		#We use a linear mapping between volume and sample index.
		sample_index = int((-1 * volume * len(self.samples)) + len(self.samples))
		channel.play(self.samples[sample_index])
	
	def mute(self):
		for sample in self.samples:
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
	kit.loadSamples("TR-808")

	def callback(message):
		print(str(message))
		channel = ((message[0] >> 2) & 0x0F)

		if (channel < 12):
			kit.play(kit.lookupChannel(channel), volume = message[1] / 255.0)
		else:
			kit.mute(kit.lookupChannel(channel))
	
	sm = SerialMonitor(callback)


'''
for x in range(0, 4):
	kit.play("ride")
	time.sleep(0.3)
	kit.play("ride", volume=0.5)
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

while pygame.mixer.get_busy():
	time.sleep(0.1)
