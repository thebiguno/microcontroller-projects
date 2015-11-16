#ifndef HIHAT_H
#define HIHAT_H

#include "Pad.h"

namespace digitalcave {

	/*
	 * A HiHat object is a special type of Pad which uses two analog input channels (pedal position and cymbal
	 * strike velocity) along with a digital switch (pedal tightly closed) to play back sounds.
	 */
	class HiHat : public Pad {
		private:
			
		public:
			//Create a new pad object referencing the specified hardware channel
			HiHat(uint8_t cymbalIndex, uint8_t pedalIndex);

			//Override filename lookup to support hihat pedal
			char* lookupFilename(uint8_t volume);
			
			//Implement virtual method
			void poll();
	};
	
}

#endif