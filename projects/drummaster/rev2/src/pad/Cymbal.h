#ifndef CYMBAL_H
#define CYMBAL_H

#include "Drum.h"

namespace digitalcave {

	/*
	 * A Cymbal object is a type of Pad which supports an analog input channel (strike velocity)
	 * along with a digital switch (mute).
	 */
	class Cymbal : public Drum {
		protected:
			uint8_t switchMuxIndex;
			
			//The gain applied to the fade algorithm for this cymbal.  0.99 results in a 
			// fairly long fade out; 0.97 is much quicker.
			double fadeGain;
			
			//The current switch value.
			uint8_t switchValue;
			//The last switch state is stored here when the new one is read.
			uint8_t lastSwitchValue;
		
			//Updates the switchValue and lastSwitchValue variables; 0 for open (not pressed), 1 for closed (pressed)
			void readSwitch(uint8_t muxIndex);

		public:
			//Create a new pad object referencing the specified hardware channel
			Cymbal(uint8_t piezoIndex, uint8_t muteIndex, uint8_t doubleHitThreshold, double fadeGain);
			
			//Returns the fade gain (either override or default)
			double getFadeGain();
			
			//Implement virtual method
			void poll();
	};
	
}

#endif