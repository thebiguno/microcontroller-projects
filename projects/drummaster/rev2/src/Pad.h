#ifndef PAD_H
#define PAD_H

#include <math.h>

#include <ADC.h>

#include "Mapping.h"
#include "Sample.h"
#include "hardware.h"

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
#define MUX_NA		0xFF

//The maximum time (in ms) between a new hit being detected and when we return
// the value.
#define MAX_RESPONSE_TIME			1

//Minimum ADC value to register as a hit
#define MIN_VALUE					16

#define PAD_TYPE_DRUM				0
#define PAD_TYPE_CYMBAL				1
#define PAD_TYPE_HIHAT				2

#define HIHAT_SPECIAL_CHIC			16
#define HIHAT_SPECIAL_SPLASH		17

//2 raised to this number is how many samples we keep in the running pedal position average
#define AVERAGE_PEDAL_COUNT_EXP		8


namespace digitalcave {

	class Pad {
		public:
			//All pads in the system.
			static Pad* pads[PAD_COUNT];

			//Initialize the ADC
			static void init();
			
			static Pad* getPad(uint8_t padIndex);
		
			//Constructor
			Pad(uint8_t padType, uint8_t piezoMuxIndex, uint8_t switchMuxIndex, uint8_t pedalMuxIndex, uint8_t doubleHitThreshold, double fadeGain);
			
			//Returns the pad type.
			uint8_t getPadType();
			
			//Method called repeatedly from main code.  If the pad was hit, start playing appropriate sample(s)
			void poll();
			
			//Get / set the per-pad volume.
			double getPadVolume();
			void setPadVolume(double padVolume);

		private:
			//ADC Object
			static ADC* adc;
			
			//Index to keep track of current index (for pad constructor).
			static uint8_t currentIndex;
			
			//Type of pad (PAD_TYPE_*)
			uint8_t padType;
			
			//The index into the pads array for this instance
			uint8_t padIndex;

			//MUX index for the piezo, switch, and pedal.  Set to MUX_NA if not applicable.
			uint8_t piezoMuxIndex;
			uint8_t switchMuxIndex;
			uint8_t pedalMuxIndex;
			
			//The gain applied to the fade algorithm for this cymbal.  0.99 results in a 
			// fairly long fade out; 0.97 is much quicker.
			double fadeGain;

			/*** State variables used in reading the pizeo value ***/
			//The time at which this hit was first read.  We must return a value within
			// at most MAX_RESPONSE_TIME ms from this time.
			uint32_t strikeTime;
			//The peak value read from the ADC for this particular strike.
			// We repeatedly read the ADC, keeping track of the peak value, until the 
			// readings have stabilized or the maximum time has passed.
			uint16_t peakValue;
			//The time at which the last value was considered stable and returned
			uint32_t playTime;
			//The last piezo value which was returned / the last raw value used to obtain the piezo value
			double lastPiezo;
			uint16_t lastRaw;

			/*** State variables used in reading switch values ***/
			//The current switch value and previous value.
			uint8_t switchValue;
			uint8_t lastSwitchValue;
			
			/*** State variables used in reading pedal position ***/
			//Measured pedal position and previous value
			uint8_t pedalPosition;
			uint8_t lastPedalPosition;
			
			//Running average of previous pedal positions; this must be divided to get the actual value
			uint16_t averagePedalPosition;
			
			
			
			//Time that the last chic was played
			uint32_t lastChicTime;
			//Volume that the last chic was played at
			double lastChicVolume;

			
			//Maximum time in ms after a sample has been played before we can play another one.
			uint8_t doubleHitThreshold;

			/*** Internal state ***/
			//The per-pad volume gain.  Limited from 0 - 5.
			double padVolume;
			
			//The last time the sample was changed based on pedal position
			uint32_t lastSampleChange;
			
			//The last sample which was played for the given file (we can adjust the volume on this if needed)
			Sample* lastSample[FILENAME_COUNT];

			/*** Private functions ***/
			//Returns the strike velocity.  Handles the ADC, draining, etc.
			double readPiezo(uint8_t muxIndex);
			//Updates the switchValue and lastSwitchValue variables; 0 for open (not pressed), 1 for closed (pressed)
			void readSwitch(uint8_t muxIndex);
			//Returns the pedal position as a number between 0x00 and 0x0F.  This includes the scaling logic needed to calibrate the pedal to actual positions.
			void readPedal(uint8_t muxIndex);
	};
	
}

#endif