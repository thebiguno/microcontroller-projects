#include "Samples.h"

using namespace digitalcave;

AudioControlSGTL5000 Samples::control;
uint8_t Samples::controlEnabled = 0;

Samples::Samples():
	samples {
		Sample(0),
		Sample(1),
		Sample(2),
		Sample(3),
		Sample(4),
		Sample(5),
		Sample(6),
		Sample(7),
		Sample(8),
		Sample(9),
		Sample(10),
		Sample(11)
	}
{
	//Nothing to see here...
}

void Samples::setMasterVolume(double volume){
	if (!controlEnabled){
		control.enable();
		controlEnabled = 1;
	}
	control.volume(volume);
}

Sample* Samples::findAvailableSample(uint8_t channel, uint8_t volume){
	//Oldest overall sample
	uint8_t oldestSample = 0;
	uint16_t oldestSamplePosition = 0;

	//Oldest sample within the same channel (that is quieter than the new sound)
	uint8_t oldestSampleByChannel = 0;
	uint16_t oldestSampleByChannelPosition = 0;
	uint8_t sampleByChannelCount = 0;
	
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++){		//TODO
		if (samples[i].isPlaying() == 0){
			return &(samples[i]);
		}
		else {
			if (samples[i].getPositionMillis() > oldestSamplePosition){
				oldestSamplePosition = samples[i].getPositionMillis();
				oldestSample = i;
			}
			if (samples[i].getLastChannel() == channel 
					&& samples[i].getGain() < volume
					&& samples[i].getPositionMillis() > oldestSampleByChannelPosition){
				oldestSampleByChannelPosition = samples[i].getPositionMillis();
				oldestSampleByChannel = i;
				sampleByChannelCount++;
			}
		}
	}
	
	if (sampleByChannelCount >= 1){
		//We don't want to kill the latest sound in this channel...
		samples[oldestSampleByChannel].stop();
		return &(samples[oldestSampleByChannel]);
	}
	else {
		//If there are no free samples and not enough playing samples in the current channel, we 
		// just pick the oldest sound and stop it.
		samples[oldestSample].stop();
		return &(samples[oldestSample]);
	}
}