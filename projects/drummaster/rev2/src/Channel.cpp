#include "Channel.h"

using namespace digitalcave;

//All the audio junk.  Static members of the class.

//Audio board control object
AudioControlSGTL5000 Channel::control;
uint8_t Channel::controlEnabled = 0;

//I2S input
AudioInputI2S Channel::input;

//Mixer
AudioMixer16 Channel::mixer;

//Output
AudioOutputI2S Channel::output;

//Input passthrough to mixer
AudioConnection Channel::inputToMixer0(input, 0, mixer, SAMPLE_COUNT);
AudioConnection Channel::inputToMixer1(input, 1, mixer, SAMPLE_COUNT + 1);

//Mixer to output
AudioConnection Channel::mixerToOutput0(mixer, 0, output, 0);
AudioConnection Channel::mixerToOutput1(mixer, 0, output, 1);

//Initialize samples array
uint8_t Channel::currentIndex = 0;
Channel Channel::samples[SAMPLE_COUNT];

/***** Static methods *****/

void Channel::setVolumeLineOut(uint8_t volume){
	if (!controlEnabled){
		control.enable();
		controlEnabled = 1;
	}

	control.volume(volume / 256.0);
}

void Channel::setVolumeLineIn(uint8_t volume){
	mixer.gain(SAMPLE_COUNT, volume / 256.0);
	mixer.gain(SAMPLE_COUNT + 1, volume / 256.0);
}

Channel* Channel::findAvailableChannel(uint8_t pad, uint8_t volume){
	//Oldest overall sample
	uint8_t oldestChannel = 0;
	uint16_t oldestChannelPosition = 0;

	//Oldest sample played from the same pad, which was quieter than the new sound
	uint8_t oldestChannelByPad = 0;
	uint16_t oldestChannelByPadPosition = 0;
	uint8_t sampleByPadCount = 0;
	
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++){
		if (samples[i].isPlaying() == 0){
			return &(samples[i]);	//If we have a sample object that is not playing currently, just use it.
		}
		else {
			if (samples[i].getPositionMillis() > oldestChannelPosition){
				oldestChannelPosition = samples[i].getPositionMillis();
				oldestChannel = i;
			}
			if (samples[i].getLastPad() == pad
					&& samples[i].getVolume() < volume
					&& samples[i].getPositionMillis() > oldestChannelByPadPosition){
				oldestChannelByPadPosition = samples[i].getPositionMillis();
				oldestChannelByPad = i;
				sampleByPadCount++;
			}
		}
	}
	
	//If we get to here, there are no available channels, so we will have to stop a playing one.
	// We try to pick the one which will cause the lease disruption.
	if (sampleByPadCount >= 2){
		//If there are at least 2 samples already playing for this pad, we can kill the oldest.
		samples[oldestChannelByPad].stop();
		return &(samples[oldestChannelByPad]);
	}
	else {
		//If there are no free samples and not enough playing samples in the current channel, we 
		// just pick the oldest sound and stop it.
		samples[oldestChannel].stop();
		return &(samples[oldestChannel]);
	}
}

/***** Instance methods *****/

Channel::Channel(): 
		mixerIndex(currentIndex), 
		lastPad(0xFF), 
		playSerialRaw(),
		playSerialRawToMixer(playSerialRaw, 0, mixer, currentIndex) {
	currentIndex++;	//Increment current index
}

void Channel::play(char* filename, uint8_t pad, uint8_t volume){
	Serial.println(filename);
	lastPad = pad;
	setVolume(volume);
	playSerialRaw.play(filename);
}

uint8_t Channel::isPlaying(){
	return playSerialRaw.isPlaying();
}

uint32_t Channel::getPositionMillis(){
	return playSerialRaw.isPlaying() ? playSerialRaw.positionMillis() : 0;
}

void Channel::stop(){
	playSerialRaw.stop();
}

uint8_t Channel::getVolume(){
	return volume;
}

void Channel::setVolume(uint8_t volume){
	this->volume = volume;
	mixer.gain(mixerIndex, max(volume / LINEAR_DIVISOR, pow(EXPONENTIAL_BASE, volume)) / VOLUME_DIVISOR);
}

uint8_t Channel::getLastPad(){
	return lastPad;
}