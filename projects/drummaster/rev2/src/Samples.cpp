#include "Samples.h"

using namespace digitalcave;

Samples::Samples():
	patchCord00(samples[0],  0, mixers[0], 0),
	patchCord01(samples[1],  0, mixers[0], 1),
	patchCord02(samples[2],  0, mixers[0], 2),
	patchCord03(samples[3],  0, mixers[0], 3),
	patchCord04(samples[4],  0, mixers[1], 0),
	patchCord05(samples[5],  0, mixers[1], 1),
	patchCord06(samples[6],  0, mixers[1], 2),
	patchCord07(samples[7],  0, mixers[1], 3),
	patchCord08(samples[8],  0, mixers[2], 0),
	patchCord09(samples[9],  0, mixers[2], 1),
	patchCord10(samples[10], 0, mixers[2], 2),
	patchCord11(samples[11], 0, mixers[2], 3),
	patchCord12(samples[12], 0, mixers[3], 0),
	patchCord13(samples[13], 0, mixers[3], 1),
	patchCord14(samples[14], 0, mixers[3], 2),
	patchCord15(samples[15], 0, mixers[3], 3),
	patchCord16(mixers[0], 0, masterMixer, 0),
	patchCord17(mixers[1], 0, masterMixer, 1),
	patchCord18(mixers[2], 0, masterMixer, 2),
	patchCord19(mixers[3], 0, masterMixer, 3),
	patchCord20(masterMixer, 0, output, 0),
	patchCord21(masterMixer, 0, output, 1)
{	
	//Allocate enough memory for audio
	AudioMemory(16);
	
	//Initialize maps
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++){
		this->sampleToChannelMap[i] = 0xFF;
	}
	for (uint8_t i = 0; i < CHANNEL_COUNT; i++){
		this->channelToSampleMap[i] = 0xFF;
	}
}


uint8_t Samples::findAvailableSample(){
	uint8_t oldestSample = 0;
	uint16_t oldestSamplePosition = 0;
	for (uint8_t i = 0; i < 12; i++){		//TODO
		if (this->samples[i].isPlaying() == 0){
			return i;
		}
		else if (this->samples[i].positionMillis() > oldestSamplePosition){
			oldestSamplePosition = this->samples[i].positionMillis();
			oldestSample = i;
		}
	}
	this->samples[oldestSample].stop();
	return oldestSample;
}

void Samples::play(uint8_t channel, uint8_t value){
	char sample[12];
	double volume = value / 256.0;
	switch(channel){
		case 0:
			snprintf(sample, sizeof(sample), "DX%02d.RAW", 0);
			break;
		case 1:
			snprintf(sample, sizeof(sample), "SN%02d.RAW", 0);
			break;
		case 2:
			snprintf(sample, sizeof(sample), "BS%02d.RAW", 0);
			break;
		case 3:
			snprintf(sample, sizeof(sample), "T1%02d.RAW", 0);
			break;
		case 4:
			snprintf(sample, sizeof(sample), "T2%02d.RAW", 0);
			break;
		case 5:
			snprintf(sample, sizeof(sample), "T3%02d.RAW", 0);
			break;
		case 6:
			snprintf(sample, sizeof(sample), "HH%02d.RAW", 0);
			break;
		case 7:
			snprintf(sample, sizeof(sample), "SP%02d.RAW", 0);
			break;
		case 8:
			snprintf(sample, sizeof(sample), "CR%02d.RAW", 0);
			break;
		case 9:
			snprintf(sample, sizeof(sample), "RD%02d.RAW", 0);
			break;
		case 10:
			snprintf(sample, sizeof(sample), "CX%02d.RAW", 0);
			break;
	}
	
	//Check if this channel was recently played.  If it was, check the value; we will increase
	// the gain for higher values, but will not change it for lower values.
	if (millis() - this->lastPlayedChannel[channel] < DOUBLE_HIT_THRESHOLD){
		//TODO Change the sample + volume to match the highest value
		return;
	}
	
	uint8_t c = this->findAvailableSample();
	this->mixers[c >> 2].gain(c & 0x03, volume / 2);
	this->samples[c].play("RD00.RAW");			//TODO Change to be dynamic
	
	this->sampleToChannelMap[c] = channel;
	this->channelToSampleMap[channel] = c;
	this->lastPlayedChannel[channel] = millis();
	
	Serial.print("Playing sample index ");
	Serial.print(c);
	Serial.print(" at volume ");
	Serial.print(volume);
	Serial.println();
	
	Serial.print("CPU: ");
	Serial.print(AudioProcessorUsage());
	Serial.print(",");
	Serial.print(AudioProcessorUsageMax());
	Serial.print("    ");
	Serial.print("Memory: ");
	Serial.print(AudioMemoryUsage());
	Serial.print(",");
	Serial.print(AudioMemoryUsageMax());
	Serial.println();
}