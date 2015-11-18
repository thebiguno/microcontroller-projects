#include "HiHat.h"

using namespace digitalcave;

HiHat::HiHat(uint8_t cymbalIndex, uint8_t pedalIndex) : Pad(cymbalIndex){
	strncpy(filenamePrefix, "HH", 2);
}

char* HiHat::lookupFilename(uint8_t volume){
	//TODO Use passed in volume (strike) along with pedal position to figure out right sample.
	return (char*) "RD_0_A.RAW";
}

void HiHat::poll(){
	uint8_t volume = readAdc();
	if (volume){
		lastSample = Sample::findAvailableSample(index, volume);
		lastSample->play(lookupFilename(volume), index, volume);
	}
}