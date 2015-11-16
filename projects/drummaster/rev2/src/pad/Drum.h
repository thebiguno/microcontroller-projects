#ifndef DRUM_H
#define DRUM_H

#include "Pad.h"

namespace digitalcave {

	/*
	 * A Drum object is a type of Pad which supports an analog input channel (strike velocity)
	 */
	class Drum : public Pad {
		private:
			
		public:
			//Create a new pad object referencing the specified hardware channel
			Drum(uint8_t index);

			//Implement virtual method
			void poll();
	};
	
}

#endif