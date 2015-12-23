#include "Sample.h"

#define FADE_OUT		1
#define FADE_IN			2

using namespace digitalcave;

//All the audio junk.  Static members of the class.

//Audio board control object
AudioControlSGTL5000 Sample::control;
uint8_t Sample::controlEnabled = 0;

//I2S input
AudioInputI2S Sample::input;

//Mixers
AudioMixer4 Sample::mixers[4];
AudioMixer4 Sample::mixer;

//Mixer connections
AudioConnection Sample::mixer0ToMixer(mixers[0], 0, mixer, 0);
AudioConnection Sample::mixer1ToMixer(mixers[1], 0, mixer, 1);
AudioConnection Sample::mixer2ToMixer(mixers[2], 0, mixer, 2);
AudioConnection Sample::mixer3ToMixer(mixers[3], 0, mixer, 3);

//Output
AudioOutputI2S Sample::output;

//Input passthrough to mixers[3], channels 2 and 3
AudioConnection Sample::inputToMixer0(input, 0, mixers[3], 2);
AudioConnection Sample::inputToMixer1(input, 1, mixers[3], 3);

//Mixer to output
AudioConnection Sample::mixerToOutput0(mixer, 0, output, 0);
AudioConnection Sample::mixerToOutput1(mixer, 0, output, 1);

//Initialize samples array
uint8_t Sample::currentIndex = 0;
Sample Sample::samples[SAMPLE_COUNT];

/***** Static methods *****/

void Sample::setVolumeLineOut(double volume){
	if (!controlEnabled){
		control.enable();
		controlEnabled = 1;
	}

	if (volume < 0.0) volume = 0.0;
	else if (volume > 1.0) volume = 1.0;
	control.volume(volume);
}

void Sample::setVolumeLineIn(double volume){
	if (volume < 0.0) volume = 0.0;
	else if (volume > 1.0) volume = 1.0;

	mixers[3].gain(2, volume);
	mixers[3].gain(3, volume);
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

void Sample::getSamplesByPad(uint8_t pad, Sample** result, uint8_t* count){
	*count = 0;
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++){
		if (samples[i].lastPad == pad && samples[i].isPlaying()){
// 			Serial.print("Sample match at index ");
// 			Serial.println(i);
			result[*count] = &(samples[i]);
			*count = *count + 1;
		}
	}
}


/***** Instance methods *****/

Sample::Sample(): 
		mixerIndex((currentIndex >> 2) & 0x03), 
		mixerChannel(currentIndex & 0x03),
		lastPad(0xFF),
		fadeGain(1),
		fading(0),
		playSerialRaw(),
		playSerialRawToMixer(playSerialRaw, 0, mixers[mixerIndex], mixerChannel),
		volume(0),
		special(0){
	currentIndex++;	//Increment current index
}

void Sample::play(char* filename, uint8_t pad, double volume){
	play(filename, pad, volume, 0, 1);
}
void Sample::play(char* filename, uint8_t pad, double volume, uint32_t positionMillis, double fadeInGain){
	if (volume < 0) volume = 0;
	else if (volume >= 5.0) volume = 5.0;

	Serial.print(millis() % 1000);
	Serial.print(" - Playing ");
	Serial.print(filename);
	Serial.print(" at volume ");
	Serial.println(volume);
	
	fading = 0;
	fadeGain = 1;
	
	lastPad = pad;
	setVolume(volume);
	if (positionMillis > 0) { //We want to fade in when not starting at the beginning of the file, to prevent audio artifacts
		mixers[mixerIndex].gain(mixerChannel, 0.01);
		startFade(fadeInGain);
	}
	playSerialRaw.play(filename, positionMillis);
	
	strncpy(this->filename, filename, sizeof(this->filename));
}

uint8_t Sample::isPlaying(){
	return playSerialRaw.isPlaying();
}

uint8_t Sample::isFadingOut(){
	return playSerialRaw.isPlaying() && fading == FADE_OUT;
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
	if (gain > 1) {
		fading = FADE_IN;
		fadeVolume = 0.01;
		if (gain > 1 && gain > fadeGain) fadeGain = gain;	//If we are already fading, keep the highest value
		Serial.print("Fading in sample at rate ");
		Serial.println(fadeGain);
	}
	else if (gain < 1){
		fading = FADE_OUT;
		fadeVolume = volume;
		if (gain < 1 && gain < fadeGain) fadeGain = gain;	//If we are already fading, keep the highest value
		Serial.print("Fading out sample at rate ");
		Serial.println(fadeGain);
	}
	else {
		Serial.println("No fade selected");
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
			s->fadeVolume = s->fadeVolume * s->fadeGain;
// 			Serial.print("Fade volume: ");
// 			Serial.println(s->fadeVolume);
			mixers[s->mixerIndex].gain(s->mixerChannel, s->fadeVolume);
			if (s->fading == FADE_OUT && s->fadeVolume <= 0.001){
				s->playSerialRaw.stop();
				s->lastPad = 0xFF;		//Once we have finished fading, we consider it valid to re-use this sample object
			}
			else if (s->fading == FADE_IN && s->fadeVolume >= s->volume){
				s->fading = 0;
				s->setVolume(s->volume);
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
	mixers[mixerIndex].gain(mixerChannel, volume);
}

uint8_t Sample::getLastPad(){
	return lastPad;
}

char* Sample::getFilename(){
	return filename;
}

uint8_t Sample::getSpecial(){
	return special;
}

void Sample::setSpecial(uint8_t special){
	this->special = special;
}
