#include "Pad.h"

#include "HiHat.h"
#include "Drum.h"
#include "Cymbal.h"

using namespace digitalcave;

ADC* Pad::adc = NULL;
Pad* Pad::pads[PAD_COUNT] = {
	//	Type	MUX Indices				DT		Fade
	new HiHat(	MUX_0, MUX_1, MUX_15,	50,		0.95),	//Hihat + Pedal
	new Drum(	MUX_2,					50),			//Snare
	new Drum(	MUX_3,					50),			//Bass
	new Drum(	MUX_4,					50),			//Tom1
	new Cymbal(	MUX_5, MUX_14,			50,		0.98),	//Crash
	new Drum(	MUX_6,					50),			//Tom2
	new Drum(	MUX_7,					50),			//Tom3
	new Cymbal(	MUX_8, MUX_13,			50,		0.97),	//Splash
	new Cymbal(	MUX_9, MUX_12,			50,		0.99),	//Ride
	new Drum(	MUX_10,					50),			//X0
	new Drum(	MUX_11,					50)				//X1
};

//Initialize static pads array
uint8_t Pad::currentIndex = 0;

void Pad::init(){
	//Initialize the ADC
	adc = new ADC();
	adc->setResolution(10);
	adc->setConversionSpeed(ADC_LOW_SPEED);
	adc->setSamplingSpeed(ADC_VERY_LOW_SPEED);
	adc->setAveraging(4);
	
	//Drain any charge which is currently in each channel
	digitalWriteFast(DRAIN_EN, MUX_ENABLE);
	digitalWriteFast(ADC_EN, MUX_ENABLE);
	for(uint8_t i = 0; i < CHANNEL_COUNT; i++){
		digitalWriteFast(MUX0, i & 0x01);
		digitalWriteFast(MUX1, i & 0x02);
		digitalWriteFast(MUX2, i & 0x04);
		digitalWriteFast(MUX3, i & 0x08);
		
		for(uint8_t i = 0; i < 5 && adc->analogRead(ADC_INPUT) > 3; i++) {
			delay(1);
		}
	}
	digitalWriteFast(ADC_EN, MUX_DISABLE);
	digitalWriteFast(DRAIN_EN, MUX_DISABLE);
}

Pad::Pad(uint8_t doubleHitThreshold) : 
		strikeTime(0),
		peakValue(0),
		playTime(0),
		lastPiezo(0),
		doubleHitThreshold(doubleHitThreshold),
		padIndex(currentIndex),
		lastSample(NULL) {
	currentIndex++;
}

void Pad::play(double volume){
	if (volume < 0) volume = 0;
	else if (volume >= 5.0) volume = 5.0;

	lastSample = Sample::findAvailableSample(padIndex, volume);
	lastSample->play(lookupFilename(volume), padIndex, volume);
}

double Pad::readPiezo(uint8_t muxIndex){
	//Disable both MUXs
	digitalWriteFast(ADC_EN, MUX_DISABLE);
	digitalWriteFast(DRAIN_EN, MUX_DISABLE);
	
	//Set Sample
	digitalWriteFast(MUX0, muxIndex & 0x01);
	digitalWriteFast(MUX1, muxIndex & 0x02);
	digitalWriteFast(MUX2, muxIndex & 0x04);
	digitalWriteFast(MUX3, muxIndex & 0x08);
	
	//Enable ADC MUX...
	digitalWriteFast(ADC_EN, MUX_ENABLE);
	
	//Wait a bit to ensure a good signal from the MUX
	delayMicroseconds(5);

	//... read value...
	uint16_t currentValue = adc->analogRead(ADC_INPUT);
	
	//... and disable MUX again
	digitalWriteFast(ADC_EN, MUX_DISABLE);

	//If we are within double trigger threshold, AND the currentValue is greater than the last played value, 
	// then we adjust the volume of the last played sample.  We don't enable thr drain this time through;
	// if the volume is stable then it will be enabled next time around, and if it is still increasing we
	// want to go through here again.
	if (playTime + doubleHitThreshold > millis() && currentValue > lastRaw && lastSample != NULL){
		double adjustedVolume = (currentValue - MIN_VALUE) / 256.0 * padVolume;
		lastSample->setVolume(adjustedVolume);
		lastRaw = currentValue;
		return 0;
	}
	
	//If we are still within the double hit threshold, OR if we are within 4x the double hit threshold 
	// time-span AND the currently read value is less than one quarter of the previous one, then we 
	// assume this is just a ghost double trigger.  Re-enable the drain each time we go through here.
	if (playTime + doubleHitThreshold > millis() 
			|| (playTime + (doubleHitThreshold * 4) > millis() && ((currentValue - MIN_VALUE) / 256.0 * padVolume) < (lastPiezo / 4))){
		digitalWriteFast(DRAIN_EN, MUX_ENABLE);
		delayMicroseconds(50);
		return 0;
	}
	
	if (currentValue < MIN_VALUE && peakValue < MIN_VALUE){
		//No hit in progress
	}
	else if (currentValue >= MIN_VALUE && peakValue == 0){
		//A new hit has started; record the time
		strikeTime = millis();
		peakValue = currentValue;
	}
	else if (currentValue > peakValue){
		//Volume is still increasing; record this as the new peak value
		peakValue = currentValue;
	}
	
	if (peakValue && (millis() - strikeTime) > MAX_RESPONSE_TIME){
		//We have timed out; send whatever the peak value currently is
		double result = (peakValue - MIN_VALUE) / 256.0 * padVolume;
		lastRaw = peakValue;
		playTime = millis();
		peakValue = 0;
		lastPiezo = result;
		return result;
	}
	
	return 0;
}

char* Pad::lookupFilename(double volume){
	//Limit volume from 0 to 1
	if (volume < 0) volume = 0;
	else if (volume >= 1.0) volume = 1.0;

	if (sampleVolumes == 0x00 || strlen(filenamePrefix) == 0x00) {
		return NULL;
	}
	
	//We find the closest match in fileCountByVolume
	int8_t closestVolume = volume * 16;		//Multiply by 16 to get into the 16 buckets of the samples
	
	//Start at the current bucket; if that is not a match, look up and down until a match is found.  At 
	// most this will take 16 tries (since there are 16 buckets)
	for(uint8_t i = 0; i < 16; i++){
		if (((closestVolume + i) <= 0x0F) && (sampleVolumes & _BV(closestVolume + i))) {
			closestVolume = closestVolume + i;
			break;
		}
		else if (((closestVolume - i) >= 0x00) && (sampleVolumes & _BV(closestVolume - i))) {
			closestVolume = closestVolume - i;
			break;
		}
	}
	
	closestVolume = closestVolume & 0x0F;

	snprintf(filenameResult, sizeof(filenameResult), "%s_%X.RAW", filenamePrefix, closestVolume);

	return filenameResult;
}

double Pad::getPadVolume(){
	return padVolume;
}

void Pad::setPadVolume(double volume){
	if (volume < 0) volume = 0;
	else if (volume >= 5.0) volume = 5.0;

	padVolume = volume;
}

void Pad::loadAllSamples(uint8_t kitIndex){
	if (kitIndex >= Mapping::getKitCount()) kitIndex = 0;

	Mapping* mapping = Mapping::getMapping(kitIndex);
	for (uint8_t i = 0; i < PAD_COUNT; i++){
		pads[i]->loadSamples(mapping->getFilenamePrefix(i));
		pads[i]->fadeOverride = mapping->getCustom(i);
	}
}

void Pad::loadSamples(char* filenamePrefix){
	//Clear the filenames
	for (uint8_t i = 0; i < sizeof(this->filenamePrefix); i++){
		this->filenamePrefix[i] = 0x00;
	}
	for (uint8_t i = 0; i < sizeof(this->filenameResult); i++){
		this->filenameResult[i] = 0x00;
	}	
	
	//The filename prefix must be at least three chars
	if (strlen(filenamePrefix) < 3) return;
	
 	strncpy(this->filenamePrefix, filenamePrefix, FILENAME_PREFIX_STRING_SIZE - 1);
	
	//Reset sampleVolumes
	sampleVolumes = 0x00;

	SerialFlash.opendir();
	while (1) {
		char filename[16];
		unsigned long filesize;

		if (SerialFlash.readdir(filename, sizeof(filename), filesize)) {
			uint8_t filenamePrefixLength = strlen(filenamePrefix);
			if (filenamePrefixLength > 6) filenamePrefixLength = 6;
			
			//Check that this filename starts with the currently assigned filename prefix
			if (strncmp(filenamePrefix, filename, filenamePrefixLength) != 0) {
				continue;
			}
			
			//Check that there is an underscore character immediately after the filename prefix.
			if (filename[filenamePrefixLength] != '_'){
				continue;	//Invalid filename, missing '_' before volume.
			}
			
			//Check that the filename volume is valid (0..F).  The volume is the second character after the prefix.
			char fileVolume = filename[filenamePrefixLength + 1];
			uint8_t volume;
			if (fileVolume >= '0' && fileVolume <= '9') volume = fileVolume - 0x30;
			else if (fileVolume >= 'A' && fileVolume <= 'F') volume = fileVolume - 0x37;
			else {
				continue;	//Invalid volume
			}

			sampleVolumes |= _BV(volume);
		} 
		else {
			break; // no more files
		}
	}
}
