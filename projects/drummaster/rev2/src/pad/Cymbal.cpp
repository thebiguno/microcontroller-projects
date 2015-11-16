#include "Cymbal.h"

using namespace digitalcave;

Cymbal::Cymbal(uint8_t index) : Pad(index){
}

void Cymbal::poll(){
	uint8_t volume = readAdc();
	if (volume){
		lastSample = Sample::findAvailableSample(index, volume);
		lastSample->play(lookupFilename(peakRawValue), index, volume);
	}
}