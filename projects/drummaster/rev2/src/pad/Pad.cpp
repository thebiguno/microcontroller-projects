#include "Pad.h"

#include "HiHat.h"
#include "Drum.h"
#include "Cymbal.h"

using namespace digitalcave;

ADC Pad::adc;
Pad* Pad::pads[PAD_COUNT];
uint8_t Pad::randomSeedCompleted = 0;

//Initialize static pads array
uint8_t Pad::currentIndex = 0;

void Pad::init(){
	adc.setResolution(12);
	adc.setConversionSpeed(ADC_LOW_SPEED);
	adc.setAveraging(8);
	
	pads[0] = new HiHat("HH", 0, 1, 15, 50);	//Hihat + Pedal
	pads[1] = new Drum("SN", 2, 25);		//Snare
	pads[2] = new Drum("BS", 3, 100);	//Bass
	pads[3] = new Drum("T1", 4, 50);		//Tom1
	pads[4] = new Cymbal("CR", 5, 14, 50);	//Crash
	pads[5] = new Drum("T2", 6, 50);		//Tom2
	pads[6] = new Drum("T3", 7, 50);		//Tom3
	pads[7] = new Cymbal("SP", 8, 13, 50);	//Splash
	pads[8] = new Cymbal("RD", 9, 12, 50);	//Ride
	pads[9] = new Drum("X0", 10, 50);	//X0
	pads[10] = new Drum("X1", 11, 50);	//X1
	
// 	Pad* Pad::pads[PAD_COUNT] = {
// 		new HiHat("HH", 0, 1, 15, 50),	//Hihat + Pedal
// 		new Drum("SN", 2, 25),			//Snare
// 		new Drum("BS", 3, 100),			//Bass
// 		new Drum("T1", 4, 50),			//Tom1
// 		new Cymbal("CR", 5, 14, 50),	//Crash
// 		new Drum("T2", 6, 50),			//Tom2
// 		new Drum("T3", 7, 50),			//Tom3
// 		new Cymbal("SP", 8, 13, 50),	//Splash
// 		new Cymbal("RD", 9, 12, 50),	//Ride
// 		new Drum("X0", 10, 50),			//X0
// 		new Drum("X1", 11, 50)			//X1
// 	};
}

Pad::Pad(const char* filenamePrefix, uint8_t doubleHitThreshold) : doubleHitThreshold(doubleHitThreshold), padIndex(currentIndex), lastSample(NULL), lastPlayTime(0), lastReadTime(0), lastRawValue(0), peakRawValue(0) {
	strncpy(this->filenamePrefix, filenamePrefix, 3);
	updateSamples();
	currentIndex++;
}

uint8_t Pad::getVolume(){
	return volume;
}

void Pad::setVolume(uint8_t volume){
	this->volume = volume;
}

void Pad::updateAllSamples(){
	for (uint8_t i = 0; i < PAD_COUNT; i++){
		pads[i]->updateSamples();
	}
}

void Pad::updateSamples(){
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
				continue;	//Ensure the filename is in a valid format, XX_V_N.RAW.  See docs/Kit Sample Organization.txt for details
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
			char fileSample = filename[5];
			uint8_t sample;
			if (fileSample >= '0' && fileSample <= '9') sample = fileSample - 0x30;
			else if (fileSample >= 'A' && fileSample <= 'F') sample = fileSample - 0x37;
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
	
	snprintf(filenameResult, sizeof(filenameResult), "%s_%X_%X.RAW", filenamePrefix, closestVolume, (uint8_t) random(fileCountByVolume[closestVolume]));
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
	int16_t rawValue = adc.analogRead(ADC_INPUT);
	
	//... and disable MUX again
	digitalWriteFast(ADC_EN, MUX_DISABLE);
	
	//If the rawValue is high, the button is not pressed (active low); if it is low, then
	// the button is pressed.
	return rawValue < 128;
}

uint8_t Pad::readPiezo(uint8_t muxIndex){
	//Disable both MUXs
	digitalWriteFast(ADC_EN, MUX_DISABLE);
	digitalWriteFast(DRAIN_EN, MUX_DISABLE);
	
	//Set Sample
	digitalWriteFast(MUX0, muxIndex & 0x01);
	digitalWriteFast(MUX1, muxIndex & 0x02);
	digitalWriteFast(MUX2, muxIndex & 0x04);
	digitalWriteFast(MUX3, muxIndex & 0x08);

	//If we are still within the double hit threshold timespan, re-enable the drain for a few microseconds each time we go through here.
	if (lastPlayTime + doubleHitThreshold > millis()){
		digitalWriteFast(DRAIN_EN, MUX_ENABLE);
		//delayMicroseconds(100);
		return 0;
	}
	
	//Enable ADC MUX...
	digitalWriteFast(ADC_EN, MUX_ENABLE);

	//TODO Unsure of whether this delay is needed... experimentation is required.
	//delayMicroseconds(100);
	
	//... read value...
	int16_t rawValue = adc.analogRead(ADC_INPUT);
	
	//... and disable MUX again
	digitalWriteFast(ADC_EN, MUX_DISABLE);
	
	//Double trigger detection.  If this hit is within doubleHitThreshold millis of the last
	// hit, then we will either adjust the previously played sample to match this new (louder)
	// volume, or we will ignore it.
	if (rawValue > MIN_VALUE && lastPlayTime + doubleHitThreshold > millis()){
//		Serial.print("Double trigger detected; ");
		lastPlayTime = millis();
		if (lastSample != NULL && lastRawValue < rawValue){
//			Serial.println("Adjusted rawValue");
			//Change the last volume to the higher (new) value and stop processing
			lastSample->setVolume(rawValue);
			//TODO Change the sample + rawValue to match the highest value
			Serial.print("Adjusted previous hit on ");
			Serial.print(filenamePrefix);
			Serial.print(" to ");
			Serial.println(rawValue);
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
		lastPeakValueSampleTime = millis();
// 		Serial.print("Setting peak value to ");
// 		Serial.print(peakRawValue);
// 		Serial.print(" (time is ");
// 		Serial.print(millis());
// 		Serial.println(")");
	}
	else if (rawValue > (peakRawValue - MIN_VALUE) && millis() - 3 < lastPeakValueSampleTime){
		//Currently read volume is less than the peak, but within MIN_VALUE of the peak and within 3 ms of the last peak value increase
// 		Serial.print("Peak value of ");
// 		Serial.print(peakRawValue);
// 		Serial.print(" is already higher than raw value of ");
// 		Serial.print(rawValue);
// 		Serial.print(" (time is ");
// 		Serial.print(millis());
// 		Serial.println(")");
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