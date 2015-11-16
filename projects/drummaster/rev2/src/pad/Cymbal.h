#ifndef CYMBAL_H
#define CYMBAL_H

#include "Pad.h"

namespace digitalcave {

	/*
	 * A Cymbal object is a type of Pad which supports an analog input channel (strike velocity)
	 * along with a digital switch (mute).
	 */
	class Cymbal : public Pad {
		private:
			
		public:
			//Create a new pad object referencing the specified hardware channel
			Cymbal(uint8_t index);
			
			//Implement virtual method
			void poll();
	};
	
}

#endif