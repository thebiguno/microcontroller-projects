#include "Samples.h"

using namespace digitalcave;

Samples::Samples():
	patchCord00(samples[0],  0, mixer, 0),
	patchCord01(samples[1],  0, mixer, 1),
	patchCord02(samples[2],  0, mixer, 2),
	patchCord03(samples[3],  0, mixer, 3),
	patchCord04(samples[4],  0, mixer, 4),
	patchCord05(samples[5],  0, mixer, 5),
	patchCord06(samples[6],  0, mixer, 6),
	patchCord07(samples[7],  0, mixer, 7),
	patchCord08(samples[8],  0, mixer, 8),
	patchCord09(samples[9],  0, mixer, 9),
	patchCord10(samples[10], 0, mixer, 10),
	patchCord11(samples[11], 0, mixer, 11),
	
	patchCord12(input, 0, mixer, 12),
	patchCord13(input, 1, mixer, 13),

	patchCord14(mixer, 0, output, 0),
	patchCord15(mixer, 0, output, 1)
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
	
	//Set the gain for the pass through channels
	this->mixer.gain(12, 0.5);
	this->mixer.gain(13, 0.5);
	
	//Search through the files on flash and see what samples are available
	SerialFlash.opendir();
	char filename[64];
	uint32_t filesize;
	
	while (SerialFlash.readdir(filename, sizeof(filename), filesize)) {
		String drum = String(filename).substring(0, 2);
		uint8_t velocity = String(filename).charAt(3) - 0x30;
		char sample = String(filename).charAt(5);
		
		if (drum == "DX") {
			//TODO
		}
	}
}

uint8_t Samples::findAvailableSample(){
	uint8_t oldestSample = 0;
	uint16_t oldestSamplePosition = 0;
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++){		//TODO
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
	this->mixer.gain(c, volume / 2);
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