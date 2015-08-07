#include "Sample.h"

using namespace digitalcave;

//All the audio junk.  I can't seem to get it into the class structure...

extern AudioPlaySerialRaw samples[SAMPLE_COUNT];
extern AudioMixer16 mixer;


Sample::Sample(uint8_t index): index(index) {
	//Nothing to see here...
}

void Sample::play(uint8_t channel, uint8_t volume){
	setGain(volume);
	Serial.println(index);
	samples[index].play("SN00.RAW");			//TODO Change to be dynamic
}

uint8_t Sample::isPlaying(){
	return samples[index].isPlaying();
}

uint32_t Sample::positionMillis(){
	return samples[index].positionMillis();
}

void Sample::stop(){
	samples[index].stop();
}

void Sample::setGain(uint8_t volume){
	mixer.gain(index, 0.5);//volume / VOLUME_DIVISOR);
}