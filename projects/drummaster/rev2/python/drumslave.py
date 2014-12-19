import pygame 
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

class Kit:
	def __init__(self, name, samplesFolder):
		self.name = name
		self.instruments = {}
		
		print("Loading Kit " + name)
		kitPath = os.path.join(samplesFolder, name)
		for instrument in os.listdir(kitPath):
			if os.path.isdir(os.path.join(kitPath, instrument)):
				print(" Loading Instrument " + instrument)
				self.instruments[instrument] = Instrument(instrument, kitPath)
			
	def play(self, instrument, volume=1.0):
		self.instruments[instrument].play(volume)
		
	def mute(self, instrument):
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


########## Main startup hooks here ##########
if (__name__=="__main__"):

	pygame.mixer.init(frequency=44100, size=-16, channels=2, buffer=1024)
	pygame.mixer.set_num_channels(16)

	kit = Kit("TR-808", "samples")

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


while pygame.mixer.get_busy():
	time.sleep(0.1)
