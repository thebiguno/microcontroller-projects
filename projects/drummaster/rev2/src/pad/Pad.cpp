#include "Pad.h"

#include "HiHat.h"
#include "Drum.h"
#include "Cymbal.h"

using namespace digitalcave;

ADC Pad::adc;
Pad* Pad::pads[PAD_COUNT] = {
	//	Type	File	MUX Indices				Double Trigger Threshold
	new HiHat(	"HH",	MUX_0, MUX_1, MUX_15,	50),	//Hihat + Pedal
	new Drum(	"SN",	MUX_2,					25),	//Snare
	new Drum(	"BS",	MUX_3,					200),	//Bass
	new Drum(	"T1",	MUX_4,					100),	//Tom1
	new Cymbal(	"CR",	MUX_5, MUX_14,			100),	//Crash
	new Drum(	"T2",	MUX_6,					100),	//Tom2
	new Drum(	"T3",	MUX_7,					100),	//Tom3
	new Cymbal(	"SP",	MUX_8, MUX_13,			100),	//Splash
	new Cymbal(	"RD",	MUX_9, MUX_12,			50),	//Ride
	new Drum(	"X0",	MUX_10,					100),	//X0
	new Drum(	"X1",	MUX_11,					100)	//X1
};

uint8_t Pad::randomSeedCompleted = 0;

//Initialize static pads array
uint8_t Pad::currentIndex = 0;

void Pad::init(){
	adc.setResolution(12);
	adc.setConversionSpeed(ADC_LOW_SPEED);
	adc.setAveraging(8);
	
	updateAllSamples();
}

void Pad::updateAllSamples(){
	for (uint8_t i = 0; i < PAD_COUNT; i++){
		pads[i]->updateSamples();
	}
}

Pad::Pad(const char* filenamePrefix, uint8_t doubleHitThreshold) : doubleHitThreshold(doubleHitThreshold), lastSample(NULL), padIndex(currentIndex) {
	strncpy(this->filenamePrefix, filenamePrefix, 3);
	currentIndex++;
}

void Pad::play(uint8_t volume){
	lastSample = Sample::findAvailableSample(padIndex, volume);
	lastSample->play(lookupFilename(volume), padIndex, volume * (padVolume / 64.0));
}

void Pad::stop(){
	Sample::stop(padIndex);
}

uint8_t Pad::getPadVolume(){
	return padVolume;
}

void Pad::setPadVolume(uint8_t padVolume){
	this->padVolume = padVolume;
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
	if (playTime + doubleHitThreshold > millis()){
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
	if (rawValue > MIN_VALUE && playTime + doubleHitThreshold > millis()){
//		Serial.print("Double trigger detected; ");
		playTime = millis();
		if (lastSample != NULL && lastValue < rawValue){
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
	
	if (rawValue < MIN_VALUE && peakValue < MIN_VALUE){
		//No hit in progress
	}
	else if (rawValue > peakValue){
		//Serial.println("Volume not stable");
		//Is the result is still increasing?  If so, wait for it to stabilize
		peakValue = rawValue;
		peakValueTime = millis();
// 		Serial.print("Setting peak value to ");
// 		Serial.print(peakValue);
// 		Serial.print(" (time is ");
// 		Serial.print(millis());
// 		Serial.println(")");
	}
	else if (rawValue > (peakValue - MIN_VALUE) && millis() - 3 < peakValueTime){
		//Currently read volume is less than the peak, but within MIN_VALUE of the peak and within 3 ms of the last peak value increase
// 		Serial.print("Peak value of ");
// 		Serial.print(peakValue);
// 		Serial.print(" is already higher than raw value of ");
// 		Serial.print(rawValue);
// 		Serial.print(" (time is ");
// 		Serial.print(millis());
// 		Serial.println(")");
	}
	else if (peakValue < MIN_VALUE){
		//If the result has stabilized, but it is less than MIN_VALUE, then ignore it
		//TODO do we need this?  Perhaps just take out MIN_VALUE altogether?
		peakValue = 0;
	}
	else {
		//The result has stabilized and it is large enough to play a sample.
		//Reset the peak value by turning on the drain MUX
		digitalWriteFast(DRAIN_EN, MUX_ENABLE);

		uint8_t result = peakValue;

		playTime = millis();
		lastValue = peakValue;
		peakValue = 0;
		
		if (!randomSeedCompleted){
			randomSeed(millis());
			randomSeedCompleted = 1;
		}
		
		return result;
	}
	
	return 0;
}