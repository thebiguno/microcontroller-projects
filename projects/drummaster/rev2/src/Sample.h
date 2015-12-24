#ifndef SAMPLE_H
#define SAMPLE_H

#include <Audio.h>
#include <SerialFlash.h>
#include <math.h>

#include "util/Mapping.h"
#include "hardware.h"

namespace digitalcave {

	/*
	 * The Sample class is a wrapper around the Teensy Audio 'AudioPlaySerialRaw' object, and 
	 * handles playing back the actual sounds in addition to controlling existing playback.
	 * There is a lots of tasks handled by this class:
	 * 1) Static objects / methods for hooking up the actual playback.  These are Teensy Audio 
	 * classes, and include things like the main volume control, mixers, and connections 
	 * between everything.
	 * 2) Static methods for finding an available Sample object and mapping between a Sample / volume
	 * tuple and a filename
	 * 3) Private instance variables for SPI audio sample and the connection between it and the mixer
 	 * 4) Methods to play a sample, set gain (volume) on a given Sample, query state, stop playback, etc.
 	 *
 	 * Note that the Sample objects can only be accessed from the singleton array samples[], via
 	 * the static findAvailableSample method.
	 */
	class Sample {
		private:
			//Control object
			static AudioControlSGTL5000 control;
			static uint8_t controlEnabled;
			
			static AudioInputI2S input;

			//Mixers
			static AudioMixer4 mixers[];	//4 mixers...
			static AudioMixer4 mixer;		//... are channeled into one for final output

			//Output
			static AudioOutputI2S output;
			
			//Connect the mixers together
			static AudioConnection mixer0ToMixer;
			static AudioConnection mixer1ToMixer;
			static AudioConnection mixer2ToMixer;
			static AudioConnection mixer3ToMixer;

			//Input passthrough to mixer
			static AudioConnection inputToMixer0;
			static AudioConnection inputToMixer1;

			//Mixer to output
			static AudioConnection mixerToOutput0;
			static AudioConnection mixerToOutput1;
			
			//Static array of samples, along with index to keep track of current index (for sample constructor).
			static uint8_t currentIndex;
			static Sample samples[];

			//Stops all currently playing samples for the selected pad immediately.  Normally you should
			// use fade() for this rather than stopping abruptly.
			static void stop(uint8_t pad);
		

			//This sample's index into mixer
			uint8_t mixerIndex;
			uint8_t mixerChannel;
			
			//The most recently played pad index.
			uint8_t lastPad;
			
			//Allow for fade out when muting cymbals
			double fadeGain;
			uint8_t fading;
			
			//SPI flash playback object
			AudioPlaySerialflashRaw playSerialRaw;

			//Connection from playSerialRaw to mixer
			AudioConnection playSerialRawToMixer;
			
			//The last filename which was played
			char filename[FILENAME_STRING_SIZE + 6];

			//The last volume value which has been set for this Sample
			double volume;
			
			//Ignore fade requests.
			uint8_t ignoreFade;
			
			//Constructing the objects should only happen during singleton array init.
			Sample();
			
		public:
			//Set the output volume.  This is a gain value from 0 to 1.
			static void setVolumeLineOut(double volume);
			
			//Set the line in volume.  This is a gain value from 0 to 1.  Keep this low unless using the line in.
			static void setVolumeLineIn(double volume);
			
			//Find the best available Sample object from the singleton array
			static Sample* findAvailableSample(uint8_t pad, double volume);
			
			//Starts fading out all currently playing samples for the selected pad.
			static void startFade(uint8_t pad, double gain);
			
			//Stops a previously started fade
			static void stopFade(uint8_t pad);
			
			//Call this repeatedly to handle the actual fading (since using an envelope object uses way too much CPU)
			static void processFade(uint8_t pad);
			
			//Start playback using this sample's SPI playback object for the given filename
			void play(char* filename, uint8_t pad, double volume);
			void play(char* filename, uint8_t pad, double volume, uint8_t ignoreFade);
			
			//Is the sample current playing?
			uint8_t isPlaying();
			
			//If the sample is playing, return the position of the sample; otherwise return 0
			uint32_t getPositionMillis();
			
			//Fade a specific sample
			void startFade(double gain);
			
			//Stops playback
			void stop();

			//Retrieves the current sample volume.  This is a floating point gain multiplier.
			double getVolume();

			//Set the sample volume.  This is a value from 0 to 255.
			void setVolume(double volume);

			//Returns the index of the last pad which initiated playback.
			uint8_t getLastPad();
			
			//Returns the last filename which was played on this sample
			char* getFilename();
	};
	
}

#endif