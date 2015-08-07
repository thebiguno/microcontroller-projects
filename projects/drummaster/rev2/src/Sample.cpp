#include "Sample.h"

using namespace digitalcave;

//All the audio junk.  I can't seem to get it into the class structure...

//Inputs
static AudioPlaySerialRaw samples[SAMPLE_COUNT];
static AudioInputI2Sslave input;

//Mixer
static AudioMixer16 mixer;

//Output
static AudioOutputI2S output;

//Samples to mixer
static AudioConnection sample0ToMixer(samples[0], 0, mixer, 0);
static AudioConnection sample1ToMixer(samples[1], 0, mixer, 1);
static AudioConnection sample2ToMixer(samples[2], 0, mixer, 2);
static AudioConnection sample3ToMixer(samples[3], 0, mixer, 3);
static AudioConnection sample4ToMixer(samples[4], 0, mixer, 4);
static AudioConnection sample5ToMixer(samples[5], 0, mixer, 5);
static AudioConnection sample6ToMixer(samples[6], 0, mixer, 6);
static AudioConnection sample7ToMixer(samples[7], 0, mixer, 7);
static AudioConnection sample8ToMixer(samples[8], 0, mixer, 8);
static AudioConnection sample9ToMixer(samples[9], 0, mixer, 9);
static AudioConnection sample10ToMixer(samples[10], 0, mixer, 10);
static AudioConnection sample11ToMixer(samples[11], 0, mixer, 11);

//Input passthrough to mixer
static AudioConnection inputToMixer0(input, 0, mixer, 12);
static AudioConnection inputToMixer1(input, 1, mixer, 13);

//Mixer to output
static AudioConnection mixerToOutput0(mixer, 0, output, 0);
static AudioConnection mixerToOutput1(mixer, 0, output, 1);

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
	Serial.print("lengthMillis: ");
	Serial.println(samples[index].lengthMillis());

	Serial.print("positionMillis: ");
	Serial.println(samples[index].positionMillis());
	return samples[index].positionMillis();
}

void Sample::stop(){
	samples[index].stop();
}

void Sample::setGain(uint8_t volume){
	mixer.gain(index, volume / VOLUME_DIVISOR);
}