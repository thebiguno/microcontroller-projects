#include "Sample.h"

using namespace digitalcave;

//All the audio junk.  Static members of the class.

//Audio board control object
AudioControlSGTL5000 Sample::control;
uint8_t Sample::controlEnabled = 0;

//I2S input
AudioInputI2S Sample::input;

//Mixer
AudioMixer16 Sample::mixer;

//Output
AudioOutputI2S Sample::output;

//Input passthrough to mixer
AudioConnection Sample::inputToMixer0(input, 0, mixer, SAMPLE_COUNT);
AudioConnection Sample::inputToMixer1(input, 1, mixer, SAMPLE_COUNT + 1);

//Mixer to output
AudioConnection Sample::mixerToOutput0(mixer, 0, output, 0);
AudioConnection Sample::mixerToOutput1(mixer, 0, output, 1);

//Initialize samples array
uint8_t Sample::currentIndex = 0;
Sample Sample::samples[SAMPLE_COUNT];

/***** Static methods *****/

void Sample::setVolumeLineOut(uint8_t volume){
	if (!controlEnabled){
		control.enable();
		controlEnabled = 1;
	}

	control.volume(volume / 256.0);
}

void Sample::setVolumeLineIn(uint8_t volume){
	mixer.gain(SAMPLE_COUNT, volume / 256.0);
	mixer.gain(SAMPLE_COUNT + 1, volume / 256.0);
}

Sample* Sample::findAvailableSample(uint8_t pad, uint8_t volume){
	//Oldest samples played on any pad, keeping track of how many are currently
	// playing on each pad.
	uint8_t oldestSample[PAD_COUNT] = {0};
	uint8_t oldestSampleCounts[PAD_COUNT] = {0};
	uint16_t oldestSamplePositions[PAD_COUNT] = {0};
	
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++){
		if (!samples[i].isPlaying()){
			return &(samples[i]);	//If we have a sample object that is not playing currently, just use it.
		}
		else {
			uint8_t sampleLastPad = samples[i].getLastPad();
			oldestSampleCounts[sampleLastPad]++;
			if (oldestSamplePositions[sampleLastPad] < samples[i].getPositionMillis()){
				oldestSamplePositions[sampleLastPad] = samples[i].getPositionMillis();
				oldestSample[sampleLastPad] = i;
			}
		}
	}
	
	//If we get to here, there are no available Samples, so we will have to stop a playing one.
	// We try to pick the one which will cause the lease disruption.
	
	//First, if there are at least 4 samples already playing for this pad, we can just kill the oldest one
	if (oldestSampleCounts[pad] >= 4){
		samples[oldestSample[pad]].stop();
		return &(samples[oldestSample[pad]]);
	}
	//If there are no free samples and not enough playing samples from the current pad, we 
	// just pick the oldest sound from the pad which has the most playing samples, and 
	// stop it.  Not the best option, but you gotta do something...
	else {
		uint8_t highestPad = 0;
		uint8_t highestPadSampleCount = 0;
		for(uint8_t i = 0; i < PAD_COUNT; i++){
			if (highestPadSampleCount < oldestSampleCounts[i]){
				highestPadSampleCount = oldestSampleCounts[i];
				highestPad = i;
			}
		}
		Serial.print("Sample pool exhaustion! Stopping sample from pad ");
		Serial.print(highestPad);
		Serial.print(" which currently has ");
		Serial.print(highestPadSampleCount);
		Serial.println(" samples playing");
		samples[oldestSample[highestPad]].stop();
		return &(samples[oldestSample[highestPad]]);
	}
}

/***** Instance methods *****/

Sample::Sample(): 
		mixerIndex(currentIndex), 
		lastPad(0xFF), 
		playSerialRaw(),
		playSerialRawToMixer(playSerialRaw, 0, mixer, currentIndex) {
	currentIndex++;	//Increment current index
}

void Sample::play(char* filename, uint8_t pad, uint8_t volume){
	Serial.print("Playing ");
	Serial.print(filename);
	Serial.print(" at volume ");
	Serial.println(volume);
	lastPad = pad;
	setVolume(volume);
	playSerialRaw.play(filename);
}

uint8_t Sample::isPlaying(){
	return playSerialRaw.isPlaying();
}

uint32_t Sample::getPositionMillis(){
	return playSerialRaw.isPlaying() ? playSerialRaw.positionMillis() : 0;
}

void Sample::stop(uint8_t pad){
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++){
		if (samples[i].lastPad == pad && samples[i].isPlaying()){
			samples[i].stop();
		}
	}
}

void Sample::stop(){
	playSerialRaw.stop();
	lastPad = 0xFF;
}

uint8_t Sample::getVolume(){
	return volume;
}

void Sample::setVolume(uint8_t volume){
	this->volume = volume;
	mixer.gain(mixerIndex, max(volume / LINEAR_DIVISOR, pow(EXPONENTIAL_BASE, volume)) / VOLUME_DIVISOR);
}

uint8_t Sample::getLastPad(){
	return lastPad;
}