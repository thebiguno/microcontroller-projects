#include "HiHat.h"

using namespace digitalcave;

HiHat::HiHat(const char* filenamePrefix, uint8_t piezoMuxIndex, uint8_t pedalMuxIndex, uint8_t switchMuxIndex, uint8_t doubleHitThreshold) : Pad(filenamePrefix, doubleHitThreshold), piezoMuxIndex(piezoMuxIndex), pedalMuxIndex(pedalMuxIndex), switchMuxIndex(switchMuxIndex) {
}

char* HiHat::lookupFilename(uint8_t volume){
	//TODO Use passed in volume (strike) along with pedal position to figure out right sample.
	return (char*) "RD_0_A.RAW";
}

void HiHat::poll(){
	uint16_t volume = readPiezo(piezoMuxIndex);
	if (volume){
		play(volume);
	}
}