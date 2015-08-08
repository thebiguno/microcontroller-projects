#include "Pad.h"

using namespace digitalcave;

extern ADC adc;

static Samples samples;

Pad::Pad(uint8_t channel) : channel(channel), lastSample(NULL), lastPlayTime(0), lastReadTime(0), lastRawValue(0), peakRawValue(0) {
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

	//If we are still within the double hit threshold timespan, re-enable the drain for a few microseconds each time we go through here.
	if (lastPlayTime + DOUBLE_HIT_THRESHOLD > millis()){
		digitalWriteFast(DRAIN_EN, 1);
		delayMicroseconds(100);
		return;
	}
	
	//Enable ADC MUX...
	digitalWriteFast(ADC_EN, 1);

	//TODO Unsure of whether this delay is needed... experimentation is required.
	delayMicroseconds(10);
	
	//... read value...
	uint8_t rawValue = adc.analogRead(ADC_INPUT);
	if (rawValue > MIN_VALUE){
		Serial.print("Channel ");
		Serial.println(channel);
		Serial.print(" read rawValue ");
		Serial.println(rawValue);
	}
	
	//... and disable MUX again
	digitalWriteFast(ADC_EN, 0);
	
	//Double trigger detection.  If this hit is within DOUBLE_HIT_THRESHOLD millis of the last
	// hit, then we will either adjust the previously played sample to match this new (louder)
	// volume, or we will ignore it.
	if (rawValue > MIN_VALUE && lastPlayTime + DOUBLE_HIT_THRESHOLD > millis()){
		Serial.print("Double trigger detected; ");
		lastPlayTime = millis();
		if (lastSample != NULL && lastRawValue < rawValue){
			Serial.println("Adjusted rawValue");
			//Change the last volume to the higher (new) value and stop processing
			lastSample->setGain(rawValue);
			//TODO Change the sample + rawValue to match the highest value
			//Serial.println("Adjusted last hit");
			return;
		}
		else {
			Serial.println("Ignoring");
			return;
		}
	}
	
	if (rawValue < MIN_VALUE && peakRawValue < MIN_VALUE){
		//No hit in progress
		return;
	}
	else if (rawValue > peakRawValue){
		Serial.println("Volume not stable");
		//Is the result is still increasing?  If so, wait for it to stabilize
		peakRawValue = rawValue;
		return;
	}
	else if (peakRawValue < MIN_VALUE){
		//If the result has stabilized, but it is less than MIN_VALUE, then ignore it
		//TODO do we need this?  Perhaps just take out MIN_VALUE altogether?
		peakRawValue = 0;
	}
	else {
		//The result has stabilized and it is large enough to play a sample.
		//Reset the peak value by turning on the drain MUX
		digitalWriteFast(DRAIN_EN, 1);

		
		//Start the sample playback

		Serial.print("Playing at rawValue ");
		Serial.print(peakRawValue);
		Serial.print("; time = ");
		Serial.println(millis());
		lastSample = samples.findAvailableSample();
		lastSample->play(channel, peakRawValue);
		lastPlayTime = millis();
		lastRawValue = peakRawValue;
		peakRawValue = 0;
		
		Serial.println((uint32_t) lastSample);
		//Wait for a bit to let the peak drain
		//delay(1);
	}
}