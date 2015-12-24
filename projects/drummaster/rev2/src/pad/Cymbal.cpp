#include "Cymbal.h"

using namespace digitalcave;

Cymbal::Cymbal(uint8_t piezoMuxIndex, uint8_t switchMuxIndex, uint8_t doubleHitThreshold, double fadeGain) : 
	Drum(piezoMuxIndex, doubleHitThreshold),
	switchMuxIndex(switchMuxIndex), 
	fadeGain(fadeGain) {
}

void Cymbal::poll(){
	readSwitch(switchMuxIndex);
	if (!lastSwitchValue && switchValue){
		Sample::startFade(padIndex, getFadeGain());
	}
	else if (lastSwitchValue && !switchValue){
		Sample::stopFade(padIndex);
	}

	double volume = readPiezo(piezoMuxIndex);
	if (volume){
		play(volume);
	}
	
	Sample::processFade(padIndex);
}

void Cymbal::readSwitch(uint8_t muxIndex){
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

double Cymbal::getFadeGain(){
	if (fadeOverride <= 9){
		return (90 + fadeOverride) / 100.0;
	}
	else {
		return fadeGain;
	}
}