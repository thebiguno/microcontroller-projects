#include "Drum.h"

using namespace digitalcave;

Drum::Drum(uint8_t index) : Pad(index){
}

void Drum::poll(){
	uint8_t volume = readAdc();
	if (volume){
		lastChannel = Channel::findAvailableChannel(index, volume);
		lastChannel->play(lookupFilename(volume), index, volume * (this->volume / 64.0));
	}
}