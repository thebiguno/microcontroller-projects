#ifndef SAMPLE_H
#define SAMPLE_H

#include <Audio.h>
#include <SerialFlash.h>
#include <play_serial_raw.h>

#define SAMPLE_COUNT				12
//TODO use an exponential function rather than a linear divisor
#define VOLUME_DIVISOR				384.0

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