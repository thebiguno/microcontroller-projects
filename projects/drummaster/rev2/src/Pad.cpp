#include "Pad.h"

using namespace digitalcave;

ADC* Pad::adc = NULL;
Pad* Pad::pads[PAD_COUNT] = {
	//		Type				Piezo	Switch	Pedal	DT		Fade
	new Pad(PAD_TYPE_HIHAT,		MUX_0,	MUX_15,	MUX_1,	50,		0.95),	//Hihat + Pedal
	new Pad(PAD_TYPE_DRUM,		MUX_2,	MUX_NA, MUX_NA,	50,		0),		//Snare
	new Pad(PAD_TYPE_DRUM,		MUX_3,	MUX_NA, MUX_NA,	50,		0),		//Bass
	new Pad(PAD_TYPE_DRUM,		MUX_4,	MUX_NA, MUX_NA,	50,		0),		//Tom1
	new Pad(PAD_TYPE_CYMBAL,	MUX_5,	MUX_14, MUX_NA,	50,		0.98),	//Crash
	new Pad(PAD_TYPE_DRUM,		MUX_6,	MUX_NA, MUX_NA,	50,		0),		//Tom2
	new Pad(PAD_TYPE_DRUM,		MUX_7,	MUX_NA, MUX_NA,	50,		0),		//Tom3
	new Pad(PAD_TYPE_CYMBAL,	MUX_8,	MUX_13, MUX_NA,	50,		0.97),	//Splash
	new Pad(PAD_TYPE_CYMBAL,	MUX_9,	MUX_12,	MUX_1,	50,		0.99),	//Ride
	new Pad(PAD_TYPE_DRUM,		MUX_10,	MUX_NA, MUX_NA,	50,		0),		//X0
	new Pad(PAD_TYPE_DRUM,		MUX_11,	MUX_NA, MUX_NA,	50,		0)		//X1
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

Pad::Pad(uint8_t padType, uint8_t piezoMuxIndex, uint8_t switchMuxIndex, uint8_t pedalMuxIndex, uint8_t doubleHitThreshold, double fadeGain) : 
		padType(padType),
		padIndex(currentIndex),
		piezoMuxIndex(piezoMuxIndex),
		switchMuxIndex(switchMuxIndex),
		pedalMuxIndex(pedalMuxIndex),
		fadeGain(fadeGain),
		strikeTime(0),
		peakValue(0),
		playTime(0),
		lastPiezo(0),
		lastRaw(0),
		switchValue(0),
		lastSwitchValue(0),
		pedalPosition(0),
		lastPedalPosition(0),
		averagePedalPosition(0),
		lastChicTime(0),
		lastChicVolume(0),
		doubleHitThreshold(doubleHitThreshold) {
	currentIndex++;
}

void Pad::poll(){
	if (getPadType() == PAD_TYPE_CYMBAL || getPadType() == PAD_TYPE_HIHAT){
		readSwitch(switchMuxIndex);
		readPedal(pedalMuxIndex);
		
		//We have just tightly closed the pedal; play the chic sound if it is fast enough.  The 
		// volume will depend on how fast it was closed (i.e. what the average position was prior to the close)
		if (getPadType() == PAD_TYPE_HIHAT && ((!lastSwitchValue && switchValue) || (pedalPosition <= 1 && lastPedalPosition > 1))){
			double volume = ((averagePedalPosition >> AVERAGE_PEDAL_COUNT_EXP) / 16.0 - 0.2);
			Serial.print("Hihat! Volume ");
			Serial.println(volume);
			
			char filenames[FILENAME_COUNT][FILENAME_STRING_SIZE];
			uint8_t filePrefixCount = Mapping::getSelectedMapping()->getFilenames(padIndex, volume, switchValue, HIHAT_SPECIAL_CHIC, filenames);

			if (volume > 0 && lastChicTime + 200 < millis()){
				for (uint8_t i = 0; i < filePrefixCount; i++){
					Sample::startFade(padIndex, 0.95);
					Sample* sample = Sample::findAvailableSample(padIndex, volume);
					sample->play(filenames[i], padIndex, volume, 1);
					lastChicTime = millis();
					lastChicVolume = volume;
				}
			}
			else {
				Sample::startFade(padIndex, fadeGain);
			}
		}
	}

	double volume = readPiezo(piezoMuxIndex);
	if (volume){
		char filenames[FILENAME_COUNT][FILENAME_STRING_SIZE];
		uint8_t filePrefixCount = Mapping::getSelectedMapping()->getFilenames(padIndex, volume, 0, pedalPosition, filenames);
		Serial.println(filePrefixCount);
		for (uint8_t i = 0; i < filePrefixCount; i++){
			Sample* sample = Sample::findAvailableSample(padIndex, volume);
			sample->play(filenames[i], padIndex, volume, 0);
		}
	}
	
	Sample::processFade(padIndex);
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
// 	if (playTime + doubleHitThreshold > millis() && currentValue > lastRaw){
// 		double adjustedVolume = (currentValue - MIN_VALUE) / 256.0 * padVolume;
// 		lastSample->setVolume(adjustedVolume);
// 		lastRaw = currentValue;
// 		return 0;
// 	}
	
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

void Pad::readSwitch(uint8_t muxIndex){
	lastSwitchValue = switchValue;
	
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
	switchValue = currentValue < 768;
}

void Pad::readPedal(uint8_t muxIndex){
	lastPedalPosition = pedalPosition;
	
	if (switchValue) {
		pedalPosition = 0x00;		//Switch state 1 means tightly closed
	}
	else {
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
		// Pedal channel goes through peak detection circuit... it would probably
		// have been fine to just use a switching channel instead of a filtered channel).
		digitalWriteFast(DRAIN_EN, MUX_ENABLE);
		
		//currentValue is a 10 bit ADC variable; we want to return a 4 bit value from 0x00-0x0F.
		// Thus we need to right shift 6 bits (10 - 4 = 6).  We do a bitwise and just to be safe.
		pedalPosition = (currentValue >> 6) & 0x0F;
		
		//We reserve position 0 for tightly closed (from the switch parameter)
		if (pedalPosition == 0) pedalPosition = 1;
	}

	//Keep a running average of 2^AVERAGE_PEDAL_COUNT_EXP previous pedal positions
	averagePedalPosition = averagePedalPosition + pedalPosition - (averagePedalPosition >> AVERAGE_PEDAL_COUNT_EXP);
}

Pad* Pad::getPad(uint8_t padIndex){
	return pads[padIndex];
}

uint8_t Pad::getPadType(){
	return padType;
}

double Pad::getPadVolume(){
	return padVolume;
}

void Pad::setPadVolume(double volume){
	if (volume < 0) volume = 0;
	else if (volume >= 5.0) volume = 5.0;

	padVolume = volume;
}

