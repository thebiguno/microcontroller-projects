#ifndef PAD_H
#define PAD_H

#include <ADC.h>

#include "../Sample.h"
#include "../hardware.h"


//Min time in millis between hits
#define DOUBLE_HIT_THRESHOLD		50

//Minimum ADC value to register as a hit
#define MIN_VALUE					15

namespace digitalcave {

	class Pad {
		protected:
			//Set this to 1 once the ramdom number generator has been seeded.  We seed it on the first 
			// pad strike with the system time; this should allow for random numbers, as the timing of 
			// the first pad strike will differ each time.
			static uint8_t randomSeedCompleted;
		
			//The hardware index associated with this pad
			uint8_t index;
			
			//The number of sample files which can be played at each volume interval.
			// We support 16 volumes (represented by a single hex digit 0..F in the filename).
			// At each volume we can have up to 255 samples, which will be chosen randomly.
			//This array is instantiated by calling updateSamples() method.
			//Most of the time the value will be either 0 or 1.
			uint8_t fileCountByVolume[16];
			
			//The last sample object which was used to play this pad.  Used in the event that we need
			// to adjust the playback on the last sample
			Sample* lastSample;
			
			//The last time this pad was hit (and a sample started)
			uint32_t lastPlayTime;
			//The last time this pad was read (regardless of result)
			uint32_t lastReadTime;
			
			//The magnitude of the last value which was played.
			uint8_t lastRawValue;
			
			//Variables used internally for getting sample filenames
			char filenamePrefix[3];
			char filenameResult[16];
			
			//We repeatedly read the ADC until we see two consecutive readings in which the latter
			// is equal or less than the former (i.e. the value is either stable or going down).
			// Until we see this, we do not start playing the sample and activate the drain.
			// This variable keeps track of the max value so far.  Once the drain is activated,
			// this value is reset.
			uint8_t peakRawValue;
			
			//Returns the strike velocity.  Handles the ADC, draining, etc.
			uint8_t readAdc();
			
		public:
			//Create a new pad object referencing the specified hardware channel
			Pad(uint8_t index);
			
			//Reads the ADC for any current activity.  Starts playing the sample if warranted.
			virtual void poll() = 0;
			
			//Looks on the SPI flash chip for files according to the sample naming convention,
			// and updates the fileCountByVolume array, which indicates which files are 
			// available.
			void updateSamples();
			
			//Find the correct sample, given a volume.
			char* lookupFilename(uint8_t volume);
	};
	
}

#endif