// #ifndef SAMPLES_H
// #define SAMPLES_H
// 
// #include <Audio.h>
// #include <SerialFlash.h>
// #include <play_serial_raw.h>
// 
// #include "DrumMaster.h"
// 
// #define SAMPLE_COUNT				12
// 
// //Min time in millis between hits
// #define DOUBLE_HIT_THRESHOLD		10
// 
// namespace digitalcave {
// 
// 	class Samples {
// 		private:
// 			//All the audio junk.  This, most notably the AudioConnection objects, 
// 			// need to be explicitly defined and can't really be automated to 
// 			// fit properly depending on the SAMPLE_COUNT.  We keep the SAMPLE_COUNT
// 			// for self-documenting purposes, but note that it can't really be changed
// 			// and expected to work without other code changes to support it.
// 			AudioPlaySerialRaw _samples[SAMPLE_COUNT];
// 			AudioMixer16 _mixer;
// 			AudioOutputI2S _output;
// 			AudioInputI2Sslave _input;
// 			
// 			//Samples to mixer
// 			AudioConnection _patchCord00;
// 			AudioConnection _patchCord01;
// 			AudioConnection _patchCord02;
// 			AudioConnection _patchCord03;
// 			AudioConnection _patchCord04;
// 			AudioConnection _patchCord05;
// 			AudioConnection _patchCord06;
// 			AudioConnection _patchCord07;
// 			AudioConnection _patchCord08;
// 			AudioConnection _patchCord09;
// 			AudioConnection _patchCord10;
// 			AudioConnection _patchCord11;
// 			
// 			//Input passthrough to mixer
// 			AudioConnection _patchCord12;
// 			AudioConnection _patchCord13;
// 
// 			//Mixer to output
// 			AudioConnection _patchCord14;
// 			AudioConnection _patchCord15;
// 
// // 			//The index of this array is the sample index;
// // 			// the value is the channel which was last played
// // 			// on the given sample.  Init'd to 0xFF.
// // 			uint8_t _sampleToChannelMap[SAMPLE_COUNT];
// // 			
// // 			//The index of this array is the channel number;
// // 			// the value is the sample index which was last 
// // 			// played for the given channel.  Init'd to 0xFF.
// // 			uint8_t _channelToSampleMap[CHANNEL_COUNT];
// // 			
// // 			//The time in millis in which this channel was last played.
// // 			// Used for double hit threshold detection.
// // 			uint32_t _lastPlayedChannel[CHANNEL_COUNT];
// // 			//The last played value.  Used in conjunction with lastPlayedChannel
// // 			uint8_t _lastPlayedValue[CHANNEL_COUNT];
// // 			
// // 			//Find the first available sample that is not currently playing; if all samples
// // 			// are playing, then find the oldest one to stop it.
// // 			uint8_t findAvailableSample();
// 			
// 			
// 		public:
// 			Samples();
// 			
// 			void play(uint8_t channel, uint8_t value);
// 
// 	};
// 	
// }
// 
// #endif