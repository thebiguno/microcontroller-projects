#include "Sample.h"

using namespace digitalcave;

//All the audio junk.  Static members of the class.

//Audio board control object
AudioControlSGTL5000 Sample::control;
uint8_t Sample::controlEnabled = 0;

//Inputs
AudioPlaySerialRaw Sample::audioPlaySerialRaws[SAMPLE_COUNT];
AudioInputI2S Sample::input;

//Mixer
AudioMixer16 Sample::mixer;

//Output
AudioOutputI2S Sample::output;

//Samples to mixer
AudioConnection Sample::sample0ToMixer(audioPlaySerialRaws[0], 0, mixer, 0);
AudioConnection Sample::sample1ToMixer(audioPlaySerialRaws[1], 0, mixer, 1);
AudioConnection Sample::sample2ToMixer(audioPlaySerialRaws[2], 0, mixer, 2);
AudioConnection Sample::sample3ToMixer(audioPlaySerialRaws[3], 0, mixer, 3);
AudioConnection Sample::sample4ToMixer(audioPlaySerialRaws[4], 0, mixer, 4);
AudioConnection Sample::sample5ToMixer(audioPlaySerialRaws[5], 0, mixer, 5);
AudioConnection Sample::sample6ToMixer(audioPlaySerialRaws[6], 0, mixer, 6);
AudioConnection Sample::sample7ToMixer(audioPlaySerialRaws[7], 0, mixer, 7);
AudioConnection Sample::sample8ToMixer(audioPlaySerialRaws[8], 0, mixer, 8);
AudioConnection Sample::sample9ToMixer(audioPlaySerialRaws[9], 0, mixer, 9);
AudioConnection Sample::sample10ToMixer(audioPlaySerialRaws[10], 0, mixer, 10);
AudioConnection Sample::sample11ToMixer(audioPlaySerialRaws[11], 0, mixer, 11);

//Input passthrough to mixer
AudioConnection Sample::inputToMixer0(input, 0, mixer, 12);
AudioConnection Sample::inputToMixer1(input, 1, mixer, 13);

//Mixer to output
AudioConnection Sample::mixerToOutput0(mixer, 0, output, 0);
AudioConnection Sample::mixerToOutput1(mixer, 0, output, 1);

//Initialize samples array
uint8_t Sample::currentIndex = 0;
Sample Sample::samples[SAMPLE_COUNT];

void Sample::setMasterVolume(double volume){
	if (!controlEnabled){
		control.enable();
		controlEnabled = 1;
	}
	control.volume(volume);
}

Sample* Sample::findAvailableSample(uint8_t channel, uint8_t volume){
	//Oldest overall sample
	uint8_t oldestSample = 0;
	uint16_t oldestSamplePosition = 0;

	//Oldest sample within the same channel (that is quieter than the new sound)
	uint8_t oldestSampleByChannel = 0;
	uint16_t oldestSampleByChannelPosition = 0;
	uint8_t sampleByChannelCount = 0;
	
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++){		//TODO
		if (samples[i].isPlaying() == 0){
			return &(samples[i]);
		}
		else {
			if (samples[i].getPositionMillis() > oldestSamplePosition){
				oldestSamplePosition = samples[i].getPositionMillis();
				oldestSample = i;
			}
			if (samples[i].getLastChannel() == channel 
					&& samples[i].getGain() < volume
					&& samples[i].getPositionMillis() > oldestSampleByChannelPosition){
				oldestSampleByChannelPosition = samples[i].getPositionMillis();
				oldestSampleByChannel = i;
				sampleByChannelCount++;
			}
		}
	}
	
	if (sampleByChannelCount >= 1){
		//We don't want to kill the latest sound in this channel...
		samples[oldestSampleByChannel].stop();
		return &(samples[oldestSampleByChannel]);
	}
	else {
		//If there are no free samples and not enough playing samples in the current channel, we 
		// just pick the oldest sound and stop it.
		samples[oldestSample].stop();
		return &(samples[oldestSample]);
	}
}

Sample::Sample(): index(currentIndex), lastChannel(0xFF) {
	currentIndex++;	//Increment current index
}

char* Sample::lookupSample(uint8_t channel, uint8_t volume){
	return (char*) "RD_0_A.RAW";
}

void Sample::play(uint8_t channel, uint8_t volume){
	lastChannel = channel;
	setGain(volume);
	audioPlaySerialRaws[index].play(lookupSample(channel, volume));
}

uint8_t Sample::isPlaying(){
	return audioPlaySerialRaws[index].isPlaying();
}

uint32_t Sample::getPositionMillis(){
	return audioPlaySerialRaws[index].positionMillis();
}

void Sample::stop(){
	audioPlaySerialRaws[index].stop();
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