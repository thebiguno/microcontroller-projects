#include "Pad.h"

using namespace digitalcave;

ADC Pad::adc;
Pad* Pad::pads[PAD_COUNT];
uint8_t Pad::randomSeedCompleted = 0;

void Pad::initAdc(){
	adc.setResolution(8);
	adc.setConversionSpeed(ADC_LOW_SPEED);
	adc.setAveraging(16);
}

Pad::Pad(uint8_t index) : index(index), lastChannel(NULL), lastPlayTime(0), lastReadTime(0), lastRawValue(0), peakRawValue(0) {
	switch(index){
		//cases 0 and 1 are handled in the HiHat subclass
		case 2:	//Snare
			strncpy(filenamePrefix, "SN", 3);
			break;
		case 3:	//Bass
			strncpy(filenamePrefix, "BS", 3);
			break;
		case 4:	//Tom1
			strncpy(filenamePrefix, "T1", 3);
			break;
		case 5:	//Crash
			strncpy(filenamePrefix, "CR", 3);
			break;
		case 6:	//Tom2
			strncpy(filenamePrefix, "T2", 3);
			break;
		case 7:	//Tom3
			strncpy(filenamePrefix, "T3", 3);
			break;
		case 8:	//Splash
			strncpy(filenamePrefix, "SP", 3);
			break;
		case 9:	//Ride
			strncpy(filenamePrefix, "RD", 3);
			break;
		case 10:	//X0
			strncpy(filenamePrefix, "X0", 3);
			break;
		case 11:	//X1
			strncpy(filenamePrefix, "X1", 3);
			break;
	}
	
	updateChannels();

}

void Pad::setVolume(uint8_t volume){
	this->volume = volume;
}

void Pad::updateChannels(){
	//Reset fileCountByVolume
	for (uint8_t i = 0x00; i <= 0x0F; i++){
		fileCountByVolume[i] = 0x00;
	}

	SerialFlash.opendir();
	while (1) {
		char filename[64];
		unsigned long filesize;

		if (SerialFlash.readdir(filename, sizeof(filename), filesize)) {
			if (strlen(filename) != 10){
				continue;	//Ensure the filename is in a valid format, XX_V_N.RAW.  See docs/Kit Channel Organization.txt for details
			}
			
			//Ensure that the first two characters in the filename are valid for this pad's filename prefix.
			if (strncmp(filenamePrefix, filename, 2) != 0) continue;
			
			//Ensure this filename's volume is a valid hexadecimal character (0..F)
			char fileVolume = filename[3];
			uint8_t volume;
			if (fileVolume >= '0' && fileVolume <= '9') volume = fileVolume - 0x30;
			else if (fileVolume >= 'A' && fileVolume <= 'F') volume = fileVolume - 0x37;
			else continue;	//Invalid volume
			
			//Ensure that this filename's sample number is a valid hexadecimal character (0..F)
			char fileChannel = filename[5];
			uint8_t sample;
			if (fileChannel >= '0' && fileChannel <= '9') sample = fileChannel - 0x30;
			else if (fileChannel >= 'A' && fileChannel <= 'F') sample = fileChannel - 0x37;
			else continue;	//Invalid volume

			//This is why we need to have the sample number continuous... we just record the highest sample number,
			// rather than looking through all of them.
			//TODO Should we show an error if the samples are not continuous?
			if (fileCountByVolume[volume] < (sample + 1)) fileCountByVolume[volume] = (sample + 1);
		} 
		else {
			break; // no more files
		}
	}
}

char* Pad::lookupFilename(uint8_t volume){
	//We find the closest match in fileCountByVolume, and if there is more than one, returns a random
	// sample number.
	volume = volume >> 4;		//Divide by 16 to get into the 16 buckets of the samples
	int8_t offset = 1;
	int8_t closestVolume = volume;
	while (fileCountByVolume[closestVolume] == 0){
		closestVolume = volume + offset;
		if (closestVolume >= 16) closestVolume = 16;
		if (closestVolume < 0) closestVolume = 0;
		if (offset > 0) offset *= -1;
		else offset += -1;
	}
	
	snprintf(filenameResult, sizeof(filenameResult), "%s_%x_%x.RAW", filenamePrefix, closestVolume, (uint8_t) random(fileCountByVolume[closestVolume]));
	return filenameResult;
}

uint8_t Pad::readAdc(){
	//Disable both MUXs
	digitalWriteFast(ADC_EN, MUX_DISABLE);
	digitalWriteFast(DRAIN_EN, MUX_DISABLE);
	
	//TODO Unsure of whether this delay is needed... experimentation is required.
	delayMicroseconds(1);
	
	//Set channel
	digitalWriteFast(MUX0, index & 0x01);
	digitalWriteFast(MUX1, index & 0x02);
	digitalWriteFast(MUX2, index & 0x04);
	digitalWriteFast(MUX3, index & 0x08);

	//TODO Unsure of whether this delay is needed... experimentation is required.
	delayMicroseconds(1);

	//If we are still within the double hit threshold timespan, re-enable the drain for a few microseconds each time we go through here.
	if (lastPlayTime + DOUBLE_HIT_THRESHOLD > millis()){
		digitalWriteFast(DRAIN_EN, MUX_ENABLE);
		delayMicroseconds(100);
		return 0;
	}
	
	//Enable ADC MUX...
	digitalWriteFast(ADC_EN, MUX_ENABLE);

	//TODO Unsure of whether this delay is needed... experimentation is required.
	delayMicroseconds(10);
	
	//... read value...
	uint8_t rawValue = adc.analogRead(ADC_INPUT);
//	if (rawValue > MIN_VALUE){
//		//Serial.print("Channel ");
//		//Serial.println(channel);
//		//Serial.print(" read rawValue ");
//		//Serial.println(rawValue);
//	}
	
	//... and disable MUX again
	digitalWriteFast(ADC_EN, MUX_DISABLE);
	
	//Double trigger detection.  If this hit is within DOUBLE_HIT_THRESHOLD millis of the last
	// hit, then we will either adjust the previously played sample to match this new (louder)
	// volume, or we will ignore it.
	if (rawValue > MIN_VALUE && lastPlayTime + DOUBLE_HIT_THRESHOLD > millis()){
//		Serial.print("Double trigger detected; ");
		lastPlayTime = millis();
		if (lastChannel != NULL && lastRawValue < rawValue){
//			Serial.println("Adjusted rawValue");
			//Change the last volume to the higher (new) value and stop processing
			lastChannel->setVolume(rawValue);
			//TODO Change the sample + rawValue to match the highest value
			//Serial.println("Adjusted last hit");
			return 0;
		}
		else {
//			Serial.println("Ignoring");
			return 0;
		}
	}
	
	if (rawValue < MIN_VALUE && peakRawValue < MIN_VALUE){
		//No hit in progress
	}
	else if (rawValue > peakRawValue){
		//Serial.println("Volume not stable");
		//Is the result is still increasing?  If so, wait for it to stabilize
		peakRawValue = rawValue;
	}
	else if (peakRawValue < MIN_VALUE){
		//If the result has stabilized, but it is less than MIN_VALUE, then ignore it
		//TODO do we need this?  Perhaps just take out MIN_VALUE altogether?
		peakRawValue = 0;
	}
	else {
		//The result has stabilized and it is large enough to play a sample.
		//Reset the peak value by turning on the drain MUX
		digitalWriteFast(DRAIN_EN, MUX_ENABLE);

		
		//Start the sample playback

		Serial.print("Playing ");
		Serial.print(index);
		Serial.print(" at volume ");
		Serial.println(peakRawValue);
//		Serial.print("; time = ");
//		Serial.println(millis());
//		lastChannel = Channel::findAvailableChannel(index, peakRawValue);
//		lastChannel->play(lookupFilename(peakRawValue), index, peakRawValue);
		uint8_t result = peakRawValue;

		lastPlayTime = millis();
		lastRawValue = peakRawValue;
		peakRawValue = 0;
		
		if (!randomSeedCompleted){
			randomSeed(millis());
			randomSeedCompleted = 1;
		}
		
		return result;
	}
	
	return 0;
}