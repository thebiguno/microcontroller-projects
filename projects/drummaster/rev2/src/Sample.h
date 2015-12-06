#ifndef SAMPLE_H
#define SAMPLE_H

#include <Audio.h>
#include <SerialFlash.h>
#include <math.h>

#include "hardware.h"

//The total number of samples allowed in the system.  Currently 12 seems to be right; more than this 
// can cause slowdowns when playing back.  If this is set to more than 14, you need to change how the
// mixer is set up, since currently we are using a 16 Sample mixer with two Samples reserved for 
// passing through i2s audio from an external source.
#define SAMPLE_COUNT				10

//The base of the exponential curve for audio mapping.  Nothing particularly scientific here, I just 
// played with a bunch of graphs until I found one that looked and sounded right.  Take note of the
// max value here (i.e. what is the number EXPONENTIAL_BASE^256); this is the maximum value that can 
// be output from the forumula.  You must pick VOLUME_DIVISOR to be a number which is larger than 
// the maximum.  I find that picking the maximum to be about 80% of VOLUME_DIVISOR works well enough.
#define EXPONENTIAL_BASE			1.020
//The volume divisor converts a number from EXPONENTIAL_BASE^rawValue to 0..1 (with a bit of headroom 
// to prevent clipping)
#define VOLUME_DIVISOR				(pow(EXPONENTIAL_BASE, 256) * 1.2)
//The linear part of the curve.  We take the max of (rawValue / LINEAR_DIVISOR) and (EXPONENTIAL_BASE^rawValue)
// as the final value (which is then scaled by the VOLUME_DIVISOR).  This value just keeps a non-zero
// value for very low velocity hits.
#define LINEAR_DIVISOR				8.0

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

			//This sample's index into mixer
			uint8_t mixerIndex;
			uint8_t mixerChannel;
			
			//The most recently played pad index.
			uint8_t lastPad;
			
			//Allow for fade out when muting cymbals
// 			AudioEffectEnvelope envelope;
			
			//SPI flash playback object
			AudioPlaySerialflashRaw playSerialRaw;

			//Connections from playSerialRaw to envelope to mixer
// 			AudioConnection playSerialRawToEnvelope;
// 			AudioConnection envelopeToMixer;
			AudioConnection playSerialRawToMixer;

			//The last volume value which has been set for this Sample
			double volume;

			//Constructing the objects should only happen during singleton array init.
			Sample();
			
		public:
			//Set the output volume.  This is a gain value from 0 to 1.
			static void setVolumeLineOut(double volume);
			
			//Set the line in volume.  This is a gain value from 0 to 1.  Keep this low unless using the line in.
			static void setVolumeLineIn(double volume);
			
			//Find the best available Sample object from the singleton array
			static Sample* findAvailableSample(uint8_t pad, double volume);
			
			//Stops all currently playing samples for the selected pad, fading out
			static void fade(uint8_t pad);
			
		
			//Start playback using this sample's SPI playback object for the given filename
			void play(char* filename, uint8_t pad, double volume);
			
			//Is the sample current playing?
			uint8_t isPlaying();
			
			//If the sample is playing, return the position of the sample; otherwise return 0
			uint32_t getPositionMillis();

			//Stops playback over time
			void fade();
			
			//Stops playback
			void stop();

			//Retrieves the current sample volume.  This is a floating point gain multiplier.
			double getVolume();

			//Set the sample volume.  This is a value from 0 to 255.
			void setVolume(double volume);

			//Returns the index of the last pad which initiated playback.
			uint8_t getLastPad();
	};
	
}

#endif