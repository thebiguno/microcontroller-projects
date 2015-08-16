#include "Sample.h"

using namespace digitalcave;

//All the audio junk.  I can't seem to get it into the class structure...

//Inputs
AudioPlaySerialRaw Sample::samples[SAMPLE_COUNT];
AudioInputI2S Sample::input;

//Mixer
AudioMixer16 Sample::mixer;

//Output
AudioOutputI2S Sample::output;

//Samples to mixer
AudioConnection Sample::sample0ToMixer(samples[0], 0, mixer, 0);
AudioConnection Sample::sample1ToMixer(samples[1], 0, mixer, 1);
AudioConnection Sample::sample2ToMixer(samples[2], 0, mixer, 2);
AudioConnection Sample::sample3ToMixer(samples[3], 0, mixer, 3);
AudioConnection Sample::sample4ToMixer(samples[4], 0, mixer, 4);
AudioConnection Sample::sample5ToMixer(samples[5], 0, mixer, 5);
AudioConnection Sample::sample6ToMixer(samples[6], 0, mixer, 6);
AudioConnection Sample::sample7ToMixer(samples[7], 0, mixer, 7);
AudioConnection Sample::sample8ToMixer(samples[8], 0, mixer, 8);
AudioConnection Sample::sample9ToMixer(samples[9], 0, mixer, 9);
AudioConnection Sample::sample10ToMixer(samples[10], 0, mixer, 10);
AudioConnection Sample::sample11ToMixer(samples[11], 0, mixer, 11);

//Input passthrough to mixer
AudioConnection Sample::inputToMixer0(input, 0, mixer, 12);
AudioConnection Sample::inputToMixer1(input, 1, mixer, 13);

//Mixer to output
AudioConnection Sample::mixerToOutput0(mixer, 0, output, 0);
AudioConnection Sample::mixerToOutput1(mixer, 0, output, 1);



Sample::Sample(uint8_t index): index(index), lastChannel(0xFF) {
	//Nothing to see here...
}

void Sample::play(uint8_t channel, uint8_t rawValue){
	lastChannel = channel;
	setGain(rawValue);
	Serial.println(index);
	//samples[index].play("SN_9_A.RAW");			//TODO Change to be dynamic
	samples[index].play("RD_0_A.RAW");			//TODO Change to be dynamic
}

uint8_t Sample::isPlaying(){
	return samples[index].isPlaying();
}

uint32_t Sample::getPositionMillis(){
	return samples[index].positionMillis();
}

void Sample::stop(){
	samples[index].stop();
}

uint8_t Sample::getGain(){
	return lastGain;
}

void Sample::setGain(uint8_t rawValue){
	lastGain = rawValue;
	double volume = max(rawValue / LINEAR_DIVISOR, pow(EXPONENTIAL_BASE, rawValue));
	mixer.gain(index, volume / VOLUME_DIVISOR);
}

uint8_t Sample::getLastChannel(){
	return lastChannel;
}