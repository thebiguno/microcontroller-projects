#include "Drum.h"

using namespace digitalcave;

Drum::Drum(uint8_t piezoMuxIndex, uint8_t doubleHitThreshold) : 
	Pad(doubleHitThreshold), 
	piezoMuxIndex(piezoMuxIndex){
}

void Drum::poll(){
	double volume = readPiezo(piezoMuxIndex);
	if (volume){
		play(volume);
	}
}