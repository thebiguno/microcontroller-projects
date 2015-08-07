#include "Pad.h"

using namespace digitalcave;

static ADC adc;
static Samples samples;

Pad::Pad(uint8_t channel) : channel(channel), lastSample(NULL), lastTime(0), lastVolume(0), peakVolume(0) {
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
	uint8_t volume = adc.analogRead(ADC_INPUT);
	
	//... and disable MUX again
	digitalWriteFast(ADC_EN, 0);
	
	//Double trigger detection.  If this hit is within DOUBLE_HIT_THRESHOLD millis of the last
	// hit, then we will either adjust the previously played sample to match this new (louder)
	// volume, or we will ignore it.
	if (lastTime + DOUBLE_HIT_THRESHOLD > millis()){
		//lastTime = millis();
		if (lastSample != NULL && lastVolume < volume){
			//Change the last volume to the higher (new) value and stop processing
			lastSample->setGain(volume);
			//TODO Change the sample + volume to match the highest value
			//Serial.println("Adjusted last hit");
			return;
		}
		else {
			//Serial.println("Ignoring double trigger");
			return;
		}
	}
	
	if (volume > peakVolume){
		//Is the result is still increasing?  If so, wait for it to stabilize
		peakVolume = volume;
		return;
	}
	else if (peakVolume < MIN_VALUE){
		//If the result has stabilized, but it is less than MIN_VALUE, then ignore it
		//TODO do we need this?  Perhaps just take out MIN_VALUE altogether?
		peakVolume = 0;
	}
	else {
		//The result has stabilized and it is large enough to play a sample.
		//Reset the peak value by turning on the drain MUX
		digitalWriteFast(DRAIN_EN, 1);

		
		//Start the sample playback

		Serial.print(millis());
		Serial.print(",");
		Serial.println(peakVolume);
		lastSample = samples.findAvailableSample();
		lastSample->play(channel, peakVolume);
		lastTime = millis();
		lastVolume = peakVolume;
		peakVolume = 0;
		
		Serial.println((uint32_t) lastSample);
		//Wait for a bit to let the peak drain
		//delay(1);
	}
}