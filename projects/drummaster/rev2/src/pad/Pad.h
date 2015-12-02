#ifndef PAD_H
#define PAD_H

#include <ADC.h>

#include "../Sample.h"
#include "../hardware.h"

//Minimum ADC value to register as a hit
#define MIN_VALUE					15

namespace digitalcave {

	class Pad {
		private:
			static ADC adc;
			
			//Maximum time in ms in which a double hit will be ignored
			uint8_t doubleHitThreshold;
	
		protected:
			//Set this to 1 once the ramdom number generator has been seeded.  We seed it on the first 
			// pad strike with the system time; this should allow for random numbers, as the timing of 
			// the first pad strike will differ each time.
			static uint8_t randomSeedCompleted;
			
			//Index to keep track of current index (for pad constructor).
			static uint8_t currentIndex;
			
			//The index into the pads array for this instance
			uint8_t padIndex;
			
			//The number of sample files which can be played at each volume interval.
			// We support 16 volumes (represented by a single hex digit 0..F in the filename).
			// At each volume we can have up to 255 samples, which will be chosen randomly.
			//This array is instantiated by calling updateSamples() method.
			//Most of the time the value will be either 0 or 1.
			uint8_t fileCountByVolume[16];
			
			//The last Sample which was used to play this pad.  Used in the event that we need
			// to adjust the playback on the last Sample
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
			int16_t peakRawValue;
			//The time that the peakRawValue was last set.
			uint32_t lastPeakValueSampleTime;
			
			//Returns the strike velocity.  Handles the ADC, draining, etc.
			uint8_t readPiezo(uint8_t muxIndex);
			
			//Returns the switch state: 0 for open (not pressed), 1 for closed (pressed)
			uint8_t readSwitch(uint8_t muxIndex);
			
			//The per-pad volume gain.  This number is divided by 64.0 and multiplied against the requested
			// volume when playing a sample.  A value of 64 results in no change; less than 64 will reduce
			// the volume, and greater than 64 will increase it.
			uint8_t volume;
			
		public:
			static Pad* pads[PAD_COUNT];
			
			//Initialize the ADC
			static void init();
		
			//Create a new pad object referencing the specified hardware Sample
			Pad(const char* filenamePrefix, uint8_t doubleHitThreshold);
			
			//Reads the ADC for any current activity.  Starts playing the sample if warranted.
			virtual void poll() = 0;
			
			//Get the per-pad volume.
			uint8_t getVolume();

			//Set the per-pad volume.
			void setVolume(uint8_t volume);
			
			//Calls updateSamples() for each Pad object defined.
			static void updateAllSamples();
			
			//Looks on the SPI flash chip for files according to the sample naming convention,
			// and updates the fileCountByVolume array, which indicates which files are 
			// available.
			void updateSamples();
			
			//Find the correct sample, given a volume.
			char* lookupFilename(uint8_t volume);
	};
	
}

#endif