#include "HiHat.h"

using namespace digitalcave;

HiHat::HiHat(uint8_t piezoMuxIndex, uint8_t pedalMuxIndex, uint8_t switchMuxIndex, uint8_t doubleHitThreshold) : Pad(doubleHitThreshold), piezoMuxIndex(piezoMuxIndex), pedalMuxIndex(pedalMuxIndex), switchMuxIndex(switchMuxIndex) {
}

void HiHat::poll(){
	double volume = readPiezo(piezoMuxIndex);
	if (volume){
		play(volume);
	}
}