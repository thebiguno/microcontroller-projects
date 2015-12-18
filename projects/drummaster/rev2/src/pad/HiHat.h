#ifndef HIHAT_H
#define HIHAT_H

#include "Cymbal.h"

namespace digitalcave {

	/*
	 * A HiHat object is a special type of Pad which uses two analog input channels (pedal position and cymbal
	 * strike velocity) along with a digital switch (pedal tightly closed) to play back sounds.
	 */
	class HiHat : public Cymbal {
		protected:
			uint8_t pedalMuxIndex;
			
			//Override the Pad implementation of this
			char* lookupFilename(double volume);
			
			//Returns the pedal position.
			double readPedal(uint8_t muxIndex);
	
		public:
			//Create a new pad object referencing the specified hardware channel
			HiHat(uint8_t piezoIndex, uint8_t pedalMuxIndex, uint8_t switchMuxIndex, uint8_t doubleHitThreshold, double fadeGain);

			//Implement virtual method
			void poll();
	};
	
}

#endif