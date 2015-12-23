#ifndef PAD_H
#define PAD_H

#include <math.h>

#include <ADC.h>

#include "../Sample.h"
#include "../hardware.h"
#include "../util/Mapping.h"

#define MUX_0		0
#define MUX_1		1
#define MUX_2		2
#define MUX_3		3
#define MUX_4		4
#define MUX_5		5
#define MUX_6		6
#define MUX_7		7
#define MUX_8		8
#define MUX_9		9
#define MUX_10		10
#define MUX_11		11
#define MUX_12		12
#define MUX_13		13
#define MUX_14		14
#define MUX_15		15

//The maximum time (in ms) between a new hit being detected and when we return
// the value.
#define MAX_RESPONSE_TIME			2

//Minimum ADC value to register as a hit
#define MIN_VALUE					16

namespace digitalcave {

	class Pad {
		private:
			//Index to keep track of current index (for pad constructor).
			static uint8_t currentIndex;
			
			/*** Variables used in reading the pizeo value ***/
			//The time at which this hit was first read.  We must return a value within
			// at most MAX_RESPONSE_TIME ms from this time.
			uint32_t strikeTime;
			//The peak value read from the ADC for this particular strike.
			// We repeatedly read the ADC, keeping track of the peak value, until the 
			// readings have stabilized or the maximum time has passed.
			uint16_t peakValue;
			//uint32_t peakValueTime;
			//The time at which the last value was considered stable and returned
			uint32_t playTime;
			//The last piezo value which was returned
			double lastPiezo;
			
			//Maximum time in ms after a sample has been played before we can play another one.
			uint8_t doubleHitThreshold;

			/*** Internal state ***/
			//The per-pad volume gain.  Limited from 0 - 5.
			double padVolume;
			
			/*** Variables used in determining proper file name ***/
			//Bit mask showing which samples are available at a given volume.
			//This value is instantiated by calling loadSamples() method.
			uint16_t sampleVolumes;


		protected:
			//ADC Object
			static ADC* adc;
			
			//Variables used internally for getting sample filenames
			char filenamePrefix[FILENAME_STRING_SIZE];
			char filenameResult[FILENAME_STRING_SIZE + 6];
			
			//The index into the pads array for this instance
			uint8_t padIndex;

			//Returns the strike velocity.  Handles the ADC, draining, etc.
			double readPiezo(uint8_t muxIndex);
			
			//Find the correct sample, given a volume.
			virtual char* lookupFilename(double volume);

			//Looks on the SPI flash chip for files according to the sample naming convention,
			// and updates the sampleVolumes bit mask, which indicates which files are 
			// available.
			virtual void loadSamples(char* filenamePrefix);
			
		public:
			//All pads in the system.
			static Pad* pads[PAD_COUNT];

			//Initialize the ADC
			static void init();
		
			//Calls loadSamples() for each Pad object defined.
			static void loadAllSamples(uint8_t kitIndex);
			
			//Constructor
			Pad(uint8_t doubleHitThreshold);
			
			//Method called repeatedly from main code
			virtual void poll() = 0;
			
			//Start plating this pad at the specified volume.
			void play(double volume);
			
			//Get the per-pad volume.
			double getPadVolume();

			//Set the per-pad volume.
			void setPadVolume(double padVolume);
	};
	
}

#endif