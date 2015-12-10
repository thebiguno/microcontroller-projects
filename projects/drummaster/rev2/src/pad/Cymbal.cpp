#include "Cymbal.h"

using namespace digitalcave;

Cymbal::Cymbal(uint8_t piezoMuxIndex, uint8_t switchMuxIndex, uint8_t doubleHitThreshold, double fadeGain) : 
	Pad(doubleHitThreshold),
	piezoMuxIndex(piezoMuxIndex),
	switchMuxIndex(switchMuxIndex), 
	fadeGain(fadeGain)
	{
}

void Cymbal::poll(){
	uint8_t muteSwitch = readSwitch(switchMuxIndex);
	if (muteSwitch){
		if (lastSample != NULL && lastSample->isPlaying()){
			fade(fadeGain);
		}
	}
	else {
		double volume = readPiezo(piezoMuxIndex);
		if (volume){
			play(volume);
		}
	}
}