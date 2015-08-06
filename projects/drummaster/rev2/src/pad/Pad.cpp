#include "Pad.h"

using namespace digitalcave;

Pad::_init::_init(){
	//TODO Does this actually work?!?!
	adc.setResolution(8);
	adc.setAveraging(4);
	adc.setConversionSpeed(ADC_LOW_SPEED);
}

Pad::Pad(uint8_t channel){
	//Set up ADC (only needs to be done once, but doing it multiple times doesn't hurt anything)
// 	adc.setResolution(8);
// 	adc.setAveraging(4);
// 	adc.setConversionSpeed(ADC_LOW_SPEED);

	this->channel = channel;
	maxAttackValue = 0;
	lastSample = 0;		//TODO
	lastTime = 0;
	lastValue = 0;
}

void Pad::poll(){
	//Disable both MUXs
	digitalWriteFast(ADC_EN, 0);
	digitalWriteFast(DRAIN_EN, 0);
	
	//TODO Unsure of whether this delay is needed... experimentation is required.
	delayMicroseconds(1);
	
	//Set channel
	digitalWriteFast(MUX0, channel & 0x01);
	digitalWriteFast(MUX1, channel & 0x02);
	digitalWriteFast(MUX2, channel & 0x04);
	digitalWriteFast(MUX3, channel & 0x08);

	//TODO Unsure of whether this delay is needed... experimentation is required.
	delayMicroseconds(1);

	//Enable ADC MUX...
	digitalWriteFast(ADC_EN, 1);

	//TODO Unsure of whether this delay is needed... experimentation is required.
	delayMicroseconds(10);
	
	//... read value...
	uint8_t result = Pad::adc.analogRead(ADC_INPUT);
	
	//... and disable MUX again
	digitalWriteFast(ADC_EN, 0);
	
	//TODO perform double hit detection here, rather than in Sample
// 	//Double trigger detection.  If this hit is within DOUBLE_HIT_THRESHOLD millis of the last
// 	// hit, then we will either adjust the previously played sample to match this new (louder)
// 	// volume, or we will ignore it.
// 	if (_lastPlayedChannel[channel] + DOUBLE_HIT_THRESHOLD > millis()){
// 		_lastPlayedChannel[channel] = millis();
// 		if (_lastPlayedValue[channel] < value){
// 			//Change the last volume to the higher (new) value and stop processing
// 			_mixer.gain(_channelToSampleMap[channel], volume / 2);
// 			//TODO Change the sample + volume to match the highest value
// 			//Serial.println("Adjusted last hit");
// 			return;
// 		}
// 		else {
// 			//Serial.println("Ignoring double trigger");
// 			return;
// 		}
// 	}
	
	if (result > maxAttackValue){
		//Is the result is still increasing?  If so, wait for it to stabilize
		maxAttackValue = result;
		return;
	}
	else if (maxAttackValue < MIN_VALUE){
		//If the result has stabilized, but it is less than MIN_VALUE, then ignore it
		//TODO do we need this?  Perhaps just take out MIN_VALUE altogether?
		maxAttackValue = 0;
	}
	else {
		//The result has stabilized and it is large enough to play a sample.
		//Reset the peak value by turning on the drain MUX
		digitalWriteFast(DRAIN_EN, 1);

		lastTime = millis();
		lastValue = maxAttackValue;
		
		//Start the sample playback
		//TODO
		
		maxAttackValue = 0;
		//Wait for a bit to let the peak drain
		//delay(1);
	}
}