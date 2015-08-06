#ifndef HIHAT_H
#define HIHAT_H

#include "Pad.h"

namespace digitalcave {

	class Pad : public Pad {
		private:
			//Keep track of which channels have already had pads assigned
			static uint8_t nextChannel;
			
			static ADC adc;
			
			//Static initializer.  See http://stackoverflow.com/questions/1197106/static-constructors-in-c-need-to-initialize-private-static-objects answer by EFraim
			static class _init {
				public:
					_init();
			} _initializer;
		
			//The hardware channel associated with this pad
			uint8_t channel;
			
			//We repeatedly read the ADC until we see two consecutive readings in which the latter
			// is equal or less than the former (i.e. the value is either stable or going down).
			// Until we see this, we do not start playing the sample and activate the drain.
			// This variable keeps track of the max value so far.  Once the drain is activated,
			// this value is reset.
			uint8_t maxAttackValue;
			
			//The last sample object which was used to play this pad.  Used in the event that we need
			// to adjust the playback on the last sample
			//TODO Sample* lastSample;
			uint8_t lastSample;
			
			//The last time this pad was hit (and a sample started)
			uint32_t lastTime;
			
			//The magnitude of the last value which was played.
			uint8_t lastValue;
			
		public:
			//Create a new pad object referencing the specified hardware channel
			Pad();
			
			//Reads the ADC for any current activity.  Uses maxAttackValue to determine if this is
			// ready to be played.  Starts playing the sample if warranted.
			void poll();

	};
	
}

#endif