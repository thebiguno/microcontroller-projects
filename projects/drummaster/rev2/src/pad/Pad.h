#ifndef PAD_H
#define PAD_H

#include <ADC.h>

#include "../Sample.h"
#include "../DrumMaster.h"


//Min time in millis between hits
#define DOUBLE_HIT_THRESHOLD		50

//Minimum ADC value to register as a hit
#define MIN_VALUE					15

namespace digitalcave {

	class Pad {
		protected:
			//The hardware channel associated with this pad
			uint8_t channel;
			
			//The last sample object which was used to play this pad.  Used in the event that we need
			// to adjust the playback on the last sample
			Sample* lastSample;
			
			//The last time this pad was hit (and a sample started)
			uint32_t lastPlayTime;
			//The last time this pad was read (regardless of result)
			uint32_t lastReadTime;
			
			//The magnitude of the last value which was played.
			uint8_t lastRawValue;
			
			//We repeatedly read the ADC until we see two consecutive readings in which the latter
			// is equal or less than the former (i.e. the value is either stable or going down).
			// Until we see this, we do not start playing the sample and activate the drain.
			// This variable keeps track of the max value so far.  Once the drain is activated,
			// this value is reset.
			uint8_t peakRawValue;
			
		public:
			//Create a new pad object referencing the specified hardware channel
			Pad(uint8_t channel);
			
			//Reads the ADC for any current activity.  Uses peakVolume to determine if this is
			// ready to be played.  Starts playing the sample if warranted.
			void poll();

	};
	
}

#endif