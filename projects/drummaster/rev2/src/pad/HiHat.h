#ifndef HIHAT_H
#define HIHAT_H

#include "Cymbal.h"

namespace digitalcave {

	/*
	 * A HiHat object is a special type of Pad which uses two analog input channels (pedal position and cymbal
	 * strike velocity) along with a digital switch (pedal tightly closed) to play back sounds.
	 */
	class HiHat : public Cymbal {
		private:
			/*** Maintain state of pedal position and tightly closed state. ***/
			//Measured pedal position with previous value
			uint8_t pedalPosition;
			uint8_t lastPedalPosition;
			
			//Running average of previous pedal positions; this must be divided to get the actual value
			uint16_t averagePedalPosition;
			
			//Time that the last chic was played
			uint32_t lastChicTime;
			//Volume that the last chic was played at
			double lastChicVolume;
			
			/*** Variables used in determining proper file name ***/
			//Multi dimensional bit mask showing which samples are available
			// at a given velocity (as bits along each 16 bit int) and at a given
			// pedal position or special effect (as entries in the array).
			//This value is instantiated by calling loadSamples() method.
			uint16_t sampleVolumes[18];
			
			//The last time the sample was changed based on pedal position
			uint32_t lastSampleChange;

			//Actual implementation of lookupFilename, which takes special hihat
			// effects such as splash and chic into account.
			char* lookupFilename(double volume, uint8_t hihatSpecial);
			
			//Return the average pedal position
			uint8_t getAveragePedalPosition();

	
		protected:
			uint8_t pedalMuxIndex;
			
			//Override the Pad implementation of this function to include pedal position logic
			char* lookupFilename(double volume);
			
			//Returns the pedal position as a number between 0x00 and 0x0F.  This includes the scaling 
			// logic needed to calibrate the pedal to actual positions.
			void readPedal(uint8_t muxIndex);
			
			//Override of the loadSamples function, needed to account for hihat pedal position 
			// in addition to velocity.
			void loadSamples(char* filenamePrefix);
	
		public:
			//Create a new pad object referencing the specified hardware channel
			HiHat(uint8_t piezoIndex, uint8_t pedalMuxIndex, uint8_t switchMuxIndex, uint8_t doubleHitThreshold, double fadeGain);

			//Implement virtual method
			void poll();
	};
	
}

#endif