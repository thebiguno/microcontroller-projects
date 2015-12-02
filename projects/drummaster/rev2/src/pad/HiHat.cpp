#include "HiHat.h"

using namespace digitalcave;

HiHat::HiHat(const char* filenamePrefix, uint8_t piezoMuxIndex, uint8_t pedalMuxIndex, uint8_t switchMuxIndex, uint8_t doubleHitThreshold) : Pad(filenamePrefix, doubleHitThreshold), piezoMuxIndex(piezoMuxIndex), pedalMuxIndex(pedalMuxIndex), switchMuxIndex(switchMuxIndex) {
	strncpy(this->filenamePrefix, filenamePrefix, 3);
}

char* HiHat::lookupFilename(uint8_t volume){
	//TODO Use passed in volume (strike) along with pedal position to figure out right sample.
	return (char*) "RD_0_A.RAW";
}

void HiHat::poll(){
	uint8_t volume = readPiezo(piezoMuxIndex);
	if (volume){
		lastSample = Sample::findAvailableSample(piezoMuxIndex, volume);
		lastSample->play(lookupFilename(volume), padIndex, volume * (this->volume / 64.0));
	}
}