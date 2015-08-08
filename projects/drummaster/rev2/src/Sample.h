#ifndef SAMPLE_H
#define SAMPLE_H

#include <Audio.h>
#include <SerialFlash.h>
#include <play_serial_raw.h>
#include <math.h>

#define SAMPLE_COUNT				12

//The volume divisor converts a number from 0..256 to 0..1 (with a bit of headroom to prevent clipping)
#define VOLUME_DIVISOR				300.0

namespace digitalcave {

	class Sample {
		private:
			//This sample's index into mixer
			uint8_t index;
			
		public:
			Sample(uint8_t index);
			
			void play(uint8_t channel, uint8_t volume);
			uint8_t isPlaying();
			uint32_t positionMillis();
			void stop();
			void setGain(uint8_t volume);

	};
	
}

#endif