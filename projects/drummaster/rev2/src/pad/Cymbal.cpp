#include "Cymbal.h"

using namespace digitalcave;

Cymbal::Cymbal(const char* filenamePrefix, uint8_t piezoMuxIndex, uint8_t switchMuxIndex, uint8_t doubleHitThreshold) : Pad(filenamePrefix, doubleHitThreshold), piezoMuxIndex(piezoMuxIndex), switchMuxIndex(switchMuxIndex){
}

void Cymbal::poll(){
	uint8_t muteSwitch = readSwitch(switchMuxIndex);
	if (muteSwitch){
		if (lastSample != NULL && lastSample->isPlaying()){
			stop();
		}
	}
	else {
		uint8_t volume = readPiezo(piezoMuxIndex);
		if (volume){
			play(volume);
		}
	}
}