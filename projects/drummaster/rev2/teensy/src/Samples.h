#ifndef SAMPLES_H
#define SAMPLES_H

#include <Audio.h>

#define SAMPLE_COUNT				16

//If 
#define DOUBLE_HIT_THRESHOLD		20

namespace digitalcave {

	class Samples {
		private:
			//All the audio junk.  This, most notably the AudioConnection objects, 
			// need to be explicitly defined and can't really be automated to 
			// fit properly depending on the SAMPLE_COUNT.  We keep the SAMPLE_COUNT
			// for self-documenting purposes, but note that it can't really be changed
			// and expected to work without other code changes to support it.
			AudioPlaySdRaw samples[SAMPLE_COUNT];
			AudioMixer4 mixers[SAMPLE_COUNT >> 2];
			AudioMixer4 masterMixer;
			AudioOutputI2S output;
			
			AudioConnection patchCord00;
			AudioConnection patchCord01;
			AudioConnection patchCord02;
			AudioConnection patchCord03;
			AudioConnection patchCord04;
			AudioConnection patchCord05;
			AudioConnection patchCord06;
			AudioConnection patchCord07;
			AudioConnection patchCord08;
			AudioConnection patchCord09;
			AudioConnection patchCord10;
			AudioConnection patchCord11;
			AudioConnection patchCord12;
			AudioConnection patchCord13;
			AudioConnection patchCord14;
			AudioConnection patchCord15;

			AudioConnection patchCord16;
			AudioConnection patchCord17;
			AudioConnection patchCord18;
			AudioConnection patchCord19;
			AudioConnection patchCord20;
			AudioConnection patchCord21;


			//The index of this array is the sample index;
			// the value is the channel which was last played
			// on the given sample.  Init'd to 0xFF.
			uint8_t sampleToChannelMap[SAMPLE_SIZE];
			
			//The index of this array is the channel number;
			// the value is the sample index which was last 
			// played for the given channel.  Init'd to 0xFF.
			uint8_t channelToSampleMap[11];
			
			//The time in millis in which this channel was last played.
			// Used for double hit threshold detection.
			uint32_t lastPlayedChannel[11];
			
			//Find the first available sample that is not currently playing; if all samples
			// are playing, then find the oldest one to stop it.
			uint8_t findAvailableSample();
			
			
		public:
			Samples();
			
			void play(uint8_t channel, uint8_t value);

	};
}

#endif