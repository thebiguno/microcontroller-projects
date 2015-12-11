#include "Pad.h"

#include "HiHat.h"
#include "Drum.h"
#include "Cymbal.h"

using namespace digitalcave;

ADC* Pad::adc = NULL;
Pad* Pad::pads[PAD_COUNT] = {
	//	Type	MUX Indices				DT		Fade
	new HiHat(	MUX_0, MUX_1, MUX_15,	75),			//Hihat + Pedal
	new Drum(	MUX_2,					75),			//Snare
	new Drum(	MUX_3,					150),			//Bass
	new Drum(	MUX_4,					100),			//Tom1
	new Cymbal(	MUX_5, MUX_14,			100,	0.98),	//Crash
	new Drum(	MUX_6,					100),			//Tom2
	new Drum(	MUX_7,					100),			//Tom3
	new Cymbal(	MUX_8, MUX_13,			100,	0.97),	//Splash
	new Cymbal(	MUX_9, MUX_12,			75,		0.99),	//Ride
	new Drum(	MUX_10,					100),			//X0
	new Drum(	MUX_11,					100)			//X1
};

uint8_t Pad::randomSeedCompleted = 0;

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
		
		do {
			delay(50);
// 			Serial.print("Draining channel ");
// 			Serial.println(i);
		} while(adc->analogRead(ADC_INPUT) > 3);
	}
	digitalWriteFast(ADC_EN, MUX_DISABLE);
	digitalWriteFast(DRAIN_EN, MUX_DISABLE);
}

Pad::Pad(uint8_t doubleHitThreshold) : 
		strikeTime(0),
		peakValue(0),
		playTime(0),
		doubleHitThreshold(doubleHitThreshold),
		lastSample(NULL),
		padIndex(currentIndex) {
	currentIndex++;
}

void Pad::play(double volume){
	if (volume < 0) volume = 0;
	else if (volume >= 5.0) volume = 5.0;

	lastSample = Sample::findAvailableSample(padIndex, volume);
	lastSample->play(lookupFilename(volume), padIndex, volume);
}

void Pad::fade(double gain){
	Sample::fade(padIndex, gain);
}

double Pad::getPadVolume(){
	return padVolume;
}

void Pad::setPadVolume(double volume){
	if (volume < 0) volume = 0;
	else if (volume >= 5.0) volume = 5.0;

	padVolume = volume;
}

void Pad::loadAllSamples(uint8_t kitIndex, Mapping* mapping){
	uint8_t totalMappingsCount = Mapping::loadKit(kitIndex, mapping);
	if (kitIndex >= totalMappingsCount) Mapping::loadKit(0, mapping);

	for (uint8_t i = 0; i < PAD_COUNT; i++){
		pads[i]->loadSamples(mapping->getFilenamePrefix(i));
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
	
// 	Serial.println(filenamePrefix);
 	strncpy(this->filenamePrefix, filenamePrefix, FILENAME_STRING_SIZE - 1);
	
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
			
// 			Serial.print(filename);
// 			Serial.print(" starts with ");
// 			Serial.print(filenamePrefixLength);
// 			Serial.print(" characters of ");
// 			Serial.println(filenamePrefix);
			
			//Check that the filename volume is valid (0..F).  The volume is the character immediately after the prefix.
			char fileVolume = filename[filenamePrefixLength + 1];
			uint8_t volume;
			if (fileVolume >= '0' && fileVolume <= '9') volume = fileVolume - 0x30;
			else if (fileVolume >= 'A' && fileVolume <= 'F') volume = fileVolume - 0x37;
			else {
// 				Serial.print("Invalid volume ");
// 				Serial.println(fileVolume);
				continue;	//Invalid volume
			}
// 			Serial.print("sampleVolume: ");
// 			Serial.println(volume);
			sampleVolumes |= _BV(volume);
			
			//If this is a hihat pad, we also are expecting another value immediately after the volume.  Verify that it
			// is valid, but don't actually store it (that is done in the HiHat class)
			//TODO Do we actually need to do this?
			if (padIndex == 0){
				char hihatPosition = filename[filenamePrefixLength + 1];
				if ((hihatPosition >= '0' && hihatPosition <= '9') || (fileVolume >= 'A' && fileVolume <= 'F')){
					//Valid
				}
				else {
					continue;	//Invalid hihat position
				}
			}
		} 
		else {
			break; // no more files
		}
	}
// 	Serial.println("loadSamples end");
}

char* Pad::lookupFilename(double volume){
	//Limit volume from 0 to 1
	if (volume < 0) volume = 0;
	else if (volume >= 1.0) volume = 1.0;

//	Serial.println(filenamePrefix);
	if (sampleVolumes == 0x00 || strlen(filenamePrefix) == 0x00) {
// 		Serial.println("No filename found");
		return NULL;
	}
	
	//We find the closest match in fileCountByVolume, and if there is more than one, returns a random
	// sample number.
	
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
// 	Serial.print("closestVolume = ");
// 	Serial.println(closestVolume);
	
	closestVolume = closestVolume & 0x0F;
	snprintf(filenameResult, sizeof(filenameResult), "%s%s%X.RAW", filenamePrefix, padIndex == 0 ? "0" : "_", closestVolume);

// 	Serial.print("Returning: ");
// 	Serial.println(filenameResult);
	
	return filenameResult;
}

uint8_t Pad::readSwitch(uint8_t muxIndex){
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

	//... read value...
	int16_t currentValue = adc->analogRead(ADC_INPUT);
	
	//... and disable MUX again
	digitalWriteFast(ADC_EN, MUX_DISABLE);
	
	//If the currentValue is high, the button is not pressed (active low); if it is low, then
	// the button is pressed.
	return currentValue < 128;
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

	//If we are still within the double hit threshold time-span, re-enable the drain 
	// each time we go through here.  This serves to both drain the stored charge and
	// to prevent double triggering.
	if (playTime + doubleHitThreshold > millis()){
		digitalWriteFast(DRAIN_EN, MUX_ENABLE);
		//Give a bit of time to drain.  To keep constant delays, this should
		// be the same as the delay prior to the ADC reading.
		delayMicroseconds(5);
		return 0;
	}
	
	//Enable ADC MUX...
	digitalWriteFast(ADC_EN, MUX_ENABLE);

	//A short delay here seems to help to read a stable volume.  10us appears fine.
	delayMicroseconds(5);
	
	//... read value...
	uint16_t currentValue = adc->analogRead(ADC_INPUT);
	if (!randomSeedCompleted && currentValue > MIN_VALUE){
		//Seed the randomizer based on the time of the first hit
		randomSeed(millis());
		randomSeedCompleted = 1;
	}
	
	//... and disable MUX again
	digitalWriteFast(ADC_EN, MUX_DISABLE);

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
		double result = peakValue / 256.0 * padVolume;
		playTime = millis();
		peakValue = 0;
		
		return result;
	}
	
	return 0;
}