#include "HiHat.h"

using namespace digitalcave;

HiHat::HiHat(uint8_t piezoMuxIndex, uint8_t pedalMuxIndex, uint8_t switchMuxIndex, uint8_t doubleHitThreshold, double fadeGain) : 
	Cymbal(piezoMuxIndex, switchMuxIndex, doubleHitThreshold, fadeGain),
	pedalMuxIndex(pedalMuxIndex){
}

void HiHat::poll(){
	double pedalPosition = readPedal(pedalMuxIndex);
	uint8_t pedalSwitch = readSwitch(switchMuxIndex);
	if (pedalSwitch){

		
	}
	
	double volume = readPiezo(piezoMuxIndex);
	if (volume){
		play(volume);
	}
}


double HiHat::readPedal(uint8_t muxIndex){
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
	
	//Enable drain for a bit to ensure quick response times (since the HiHat 
	// Pedal channel goes through peak detection circuit)
	digitalWriteFast(DRAIN_EN, MUX_ENABLE);
	delayMicroseconds(10);
	
	return currentValue;
}

char* HiHat::lookupFilename(double volume){
	Serial.println("Using HiHat function");
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
	char hihatVariable[2] = {'0', 0x00};
// 	if (hiHatState == 0) hiHatVariable = "_";
// 	else if (hiHatState == 0xFF) hiHatVariable = "K";
// 	else if (hiHatState <{
// 		
// 	}
	snprintf(filenameResult, sizeof(filenameResult), "%s%s%X.RAW", filenamePrefix, hihatVariable, closestVolume);

// 	Serial.print("Returning: ");
// 	Serial.println(filenameResult);
	
	return filenameResult;
}