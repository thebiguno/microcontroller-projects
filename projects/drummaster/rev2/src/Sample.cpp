#include "Sample.h"

using namespace digitalcave;

//All the audio junk.  Static members of the class.

//Audio board control object
AudioControlSGTL5000 Sample::control;
uint8_t Sample::controlEnabled = 0;
uint8_t Sample::volumeHeadphones;
uint8_t Sample::volumeLineIn;

//I2S input
AudioInputI2S Sample::input;

//Output
AudioOutputI2S Sample::output;

//Mixers
AudioMixer16 Sample::sampleMixer;
AudioMixer4 Sample::outputMixer;

//Sample mixer to output mixer
AudioConnection Sample::sampleMixerToOutputMixer(sampleMixer, 0, outputMixer, 2);

//Pass through line in to outputMixer
AudioConnection Sample::inputToOutputMixer0(input, 0, outputMixer, 0);
AudioConnection Sample::inputToOutputMixer1(input, 1, outputMixer, 1);

//Output Mixer to output
AudioConnection Sample::mixerToOutput0(outputMixer, 0, output, 0);
AudioConnection Sample::mixerToOutput1(outputMixer, 0, output, 1);

//Initialize samples array
uint8_t Sample::currentIndex = 0;
Sample Sample::samples[SAMPLE_COUNT];

/***** Static methods *****/

uint8_t Sample::getVolumeHeadphones(){
	return volumeHeadphones;
}

void Sample::setVolumeHeadphones(uint8_t volume){
	if (!controlEnabled){
		control.enable();
		controlEnabled = 1;
	}

	if (volume > 100) volume = 100;
	control.volume(volume / 100.0);
	volumeHeadphones = volume;
}

uint8_t Sample::getVolumeLineIn(){
	return volumeLineIn;
}

void Sample::setVolumeLineIn(uint8_t volume){
	if (volume > 200) volume = 200;

	outputMixer.gain(0, volume / 100.0);
	outputMixer.gain(1, volume / 100.0);
	
	volumeLineIn = volume;
}

Sample* Sample::findAvailableSample(uint8_t pad, double volume){
	//Oldest samples played on any pad, keeping track of how many are currently
	// playing on each pad.
	uint8_t sampleCounts[PAD_COUNT] = {0};
	uint8_t quietestSample[PAD_COUNT] = {0};
	double quietestSampleVolumes[PAD_COUNT] = {0};
	uint8_t oldestSample[PAD_COUNT] = {0};
	uint16_t oldestSamplePositions[PAD_COUNT] = {0};
	
	//Loop through all samples.  If we find one that is not currently playing, play it.
	// Build a structure keeping track of the quietest / oldest samples so that we can
	// stop them if there are no available samples.
	for (uint8_t sampleIndex = 0; sampleIndex < SAMPLE_COUNT; sampleIndex++){
		if (!samples[sampleIndex].isPlaying()){
			samples[sampleIndex].stop();
			return &(samples[sampleIndex]);	//If we have a sample object that is not playing currently, just use it.
		}
		else {
			uint8_t sampleLastPad = samples[sampleIndex].getLastPad();
			sampleCounts[sampleLastPad]++;
			if (oldestSamplePositions[sampleLastPad] < samples[sampleIndex].getPositionMillis()){
				oldestSamplePositions[sampleLastPad] = samples[sampleIndex].getPositionMillis();
				oldestSample[sampleLastPad] = sampleIndex;
			}
			if (quietestSampleVolumes[sampleLastPad] < samples[sampleIndex].getVolume()){
				quietestSampleVolumes[sampleLastPad] = samples[sampleIndex].getVolume();
				quietestSample[sampleLastPad] = sampleIndex;
			}
		}
	}
	
	//If we get to here, there are no available samples, so we will have to stop one that
	// is currently playing.  We try to pick the one which will cause the lease disruption.
	
	//First, if there are at least 6 samples already playing for this pad, we can just kill 
	// the quietest one, assuming it is quieter than 1/2 of the current one.
// 	if (sampleCounts[pad] >= 6 && quietestSampleVolumes[pad] < (volume * 0.5)){
// 		Serial.print("Stopping sample from pad ");
// 		Serial.print(pad);
// 		Serial.println(" as option 1");
// 		samples[quietestSample[pad]].stop();
// 		return &(samples[quietestSample[pad]]);
// 	}
// 	
// 	//Next, we look for other pads with at least 6 samples, and kill the quietest
// 	// one, if the volume is less than half of the new one.
// 	for(uint8_t i = 0; i < PAD_COUNT; i++){
// 		if (sampleCounts[i] >= 6 && quietestSampleVolumes[i] < (volume * 0.5)){
// 			Serial.print("Stopping sample from pad ");
// 			Serial.print(i);
// 			Serial.println(" as option 2");
// 			samples[quietestSample[i]].stop();
// 			return &(samples[quietestSample[i]]);
// 		}
// 	}
// 	
// 	//Next we look for any pad that has at least 4 samples, and kill the quietest one
// 	// regardless of its volume relative to the new one.
// 	for(uint8_t i = 0; i < PAD_COUNT; i++){
// 		if (sampleCounts[i] >= 4){
// 			Serial.print("Stopping sample from pad ");
// 			Serial.print(i);
// 			Serial.println(" as option 3");
// 			samples[quietestSample[i]].stop();
// 			return &(samples[quietestSample[i]]);
// 		}
// 	}
// 	
	//If there are any pads which have at least 2 samples with the same sample for
	// both the oldest and the quietest, then stop that one.
	for(uint8_t i = 0; i < PAD_COUNT; i++){
		if (sampleCounts[i] >= 2 && quietestSample[i] == oldestSample[i]){
// 			Serial.print("Stopping sample from pad ");
// 			Serial.print(i);
// 			Serial.println(" as option 4");
			samples[quietestSample[i]].stop();
			return &(samples[quietestSample[i]]);
		}
	}

	//If there are no free samples and not enough playing samples from the current pad, we 
	// just find the pad which has the most playing samples, and stop the oldest.  Not the
	// best option, but you gotta do something...
	uint8_t highestPad = 0;
	uint8_t highestPadSampleCount = 0;
	for(uint8_t i = 0; i < PAD_COUNT; i++){
		if (highestPadSampleCount < sampleCounts[i]){
			highestPadSampleCount = sampleCounts[i];
			highestPad = i;
		}
	}
// 	Serial.print("Stopping sample from pad ");
// 	Serial.print(highestPad);
// 	Serial.println(" as option 5");

	samples[oldestSample[highestPad]].stop();
	return &(samples[oldestSample[highestPad]]);
}


/***** Instance methods *****/

Sample::Sample(): 
		index(currentIndex & 0x0F), 
		playSerialRaw(),
		playSerialRawToMixer(playSerialRaw, 0, sampleMixer, index),
		lastPad(0xFF),
		fadeGain(1),
		fading(0),
		volume(0){
	currentIndex++;	//Increment current index
}

void Sample::play(char* filename, uint8_t pad, double volume){
	play(filename, pad, volume, 0);
}

void Sample::play(char* filename, uint8_t pad, double volume, uint8_t ignoreFade){
	if (volume < 0) volume = 0;
	else if (volume >= 5.0) volume = 5.0;
	
	this->ignoreFade = ignoreFade;
	fading = 0;
	fadeGain = 1;
	
	lastPad = pad;
	setVolume(volume);
	//Try to find the proper filename, given supported extensions
	if (!playSerialRaw.play(filename)){
		uint8_t filenameLength = strlen(filename);
		filename[filenameLength-3] = 'U';
		filename[filenameLength-2] = 'L';
		filename[filenameLength-1] = 'W';
		if (!playSerialRaw.play(filename)){
			filename[filenameLength-4] = 0;
			Serial.print(filename);
			Serial.println(": File not found");
			return;
		}
	}
	

//  	Serial.print(millis() % 1000);
	Serial.print("Playing ");
	Serial.print(filename);
	Serial.print(" at volume ");
	Serial.println(volume);
	
	strncpy(this->filename, filename, sizeof(this->filename));
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

void Sample::startFade(uint8_t pad, double gain){
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++){
		if (samples[i].lastPad == pad && samples[i].isPlaying()){
			samples[i].startFade(gain);
		}
	}
}

void Sample::startFade(double gain){
	if (ignoreFade) return;
	
	if (gain < 1){
		fading = 1;
		if (gain < 1 && gain < fadeGain) fadeGain = gain;	//If we are already fading, keep the highest value
// 		Serial.print("Fading out sample at rate ");
// 		Serial.println(fadeGain);
	}
	else {
// 		Serial.println("No fade selected");
		fading = 0;
		fadeGain = 1;
	}
}

void Sample::stopFade(uint8_t pad){
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++){
		if (samples[i].lastPad == pad && samples[i].isPlaying()){
			samples[i].fading = 0;
			samples[i].fadeGain = 1;
		}
	}
}

void Sample::processFade(uint8_t pad){
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++){
		Sample* s = &samples[i];
		if (s->lastPad == pad && s->isPlaying() && s->fading){
			s->volume = s->volume * s->fadeGain;
			sampleMixer.gain(s->index, s->volume);
			if (s->volume <= 0.001){
				s->playSerialRaw.stop();
				s->lastPad = 0xFF;		//Once we have finished fading, we consider it valid to re-use this sample object
			}
		}
	}
}

void Sample::stop(){
	playSerialRaw.stop();
	lastPad = 0xFF;
	fading = 0;
}

double Sample::getVolume(){
	return volume;
}

void Sample::setVolume(double volume){
	if (volume < 0) volume = 0;
	else if (volume >= 5.0) volume = 5.0;
	
	this->volume = volume;
	sampleMixer.gain(index, volume);
}

uint8_t Sample::getLastPad(){
	return lastPad;
}
