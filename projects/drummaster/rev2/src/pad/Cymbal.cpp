#include "Cymbal.h"

using namespace digitalcave;

Cymbal::Cymbal(uint8_t index) : Pad(index){
}

void Cymbal::poll(){
	uint8_t volume = readAdc();
	if (volume){
		lastChannel = Channel::findAvailableChannel(index, volume);
		lastChannel->play(lookupFilename(volume), index, volume * (this->volume / 64.0));
	}
}