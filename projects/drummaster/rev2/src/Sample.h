#ifndef SAMPLE_H
#define SAMPLE_H

#include <Audio.h>
#include <SerialFlash.h>
#include <play_serial_raw.h>

#define SAMPLE_COUNT				12

//Min time in millis between hits
#define DOUBLE_HIT_THRESHOLD		10

namespace digitalcave {

	class Sample {
		private:
			//All the audio junk.  Some is shared (static) and others are private.
			//Mixer
			static AudioMixer16 mixer;
			AudioMixer16 mixer2;

			//Outputs
			static AudioOutputI2S output;
			static AudioInputI2Sslave input;

			//Input passthrough to mixer
			static AudioConnection* inputToMixer0;
			static AudioConnection* inputToMixer1;

			//Mixer to output
			static AudioConnection* mixerToOutput0;
			static AudioConnection* mixerToOutput1;

			//Static initializer.  See http://stackoverflow.com/questions/1197106/static-constructors-in-c-need-to-initialize-private-static-objects answer by EFraim
			static class _init {
				public:
					_init();
			} _initializer;

			//The Audio object which is played by this sample
			AudioPlaySerialRaw sample;
			
			//This sample's index into mixer
			uint8_t mixerIndex;
			
			//Patchcord to the mixer
			AudioConnection* patchCord;
// 			
// 			//The index of this array is the sample index;
// 			// the value is the channel which was last played
// 			// on the given sample.  Init'd to 0xFF.
// 			uint8_t _sampleToChannelMap[SAMPLE_COUNT];
// 			
// 			//The index of this array is the channel number;
// 			// the value is the sample index which was last 
// 			// played for the given channel.  Init'd to 0xFF.
// 			uint8_t _channelToSampleMap[CHANNEL_COUNT];
// 			
// 			//The time in millis in which this channel was last played.
// 			// Used for double hit threshold detection.
// 			uint32_t _lastPlayedChannel[CHANNEL_COUNT];
// 			//The last played value.  Used in conjunction with lastPlayedChannel
// 			uint8_t _lastPlayedValue[CHANNEL_COUNT];
// 
// 			//Find the first available sample that is not currently playing; if all samples
// 			// are playing, then find the oldest one to stop it.
// 			static uint8_t findAvailableSample();
			
			
		public:
			Sample(uint8_t mixerIndex);
			
			void play(uint8_t channel, uint8_t value);

	};
	
}

#endif