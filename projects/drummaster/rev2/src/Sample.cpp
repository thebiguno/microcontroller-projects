#include "Sample.h"

using namespace digitalcave;

//All the audio junk.  I can't seem to get it into the class structure...

extern AudioPlaySerialRaw samples[SAMPLE_COUNT];
extern AudioMixer16 mixer;


Sample::Sample(uint8_t index): index(index) {
	//Nothing to see here...
}

void Sample::play(uint8_t channel, uint8_t rawValue){
	setGain(rawValue);
	Serial.println(index);
	samples[index].play("RD00.RAW");			//TODO Change to be dynamic
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

void Sample::setGain(uint8_t rawValue){
	double volume = max(rawValue / 5.0, pow(1.022, rawValue));
	mixer.gain(index, volume / VOLUME_DIVISOR);
}