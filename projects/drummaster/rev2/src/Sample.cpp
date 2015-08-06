#include "Sample.h"

using namespace digitalcave;

AudioMixer16 Sample::mixer;

Sample::_init::_init(){
	Sample::inputToMixer0 = new AudioConnection(input, 0, mixer, 12);
	Sample::inputToMixer1 = new AudioConnection(input, 1, mixer, 13);

	Sample::mixerToOutput0 = new AudioConnection(mixer, 0, output, 0);
	Sample::mixerToOutput1 = new AudioConnection(mixer, 0, output, 1);

	//Allocate enough memory for audio
	AudioMemory(16);
}

Sample::Sample(uint8_t mixerIndex):
	patchCord(sample, 0, mixer, mixerIndex)
{
	this->mixerIndex = mixerIndex;
}

void Sample::play(uint8_t channel, uint8_t value){
	double volume = value / 256.0;
// 	switch(channel){
// 		case 0:
// 			snprintf(sample, sizeof(sample), "DX%02d.RAW", 0);
// 			break;
// 		case 1:
// 			snprintf(sample, sizeof(sample), "SN%02d.RAW", 0);
// 			break;
// 		case 2:
// 			snprintf(sample, sizeof(sample), "BS%02d.RAW", 0);
// 			break;
// 		case 3:
// 			snprintf(sample, sizeof(sample), "T1%02d.RAW", 0);
// 			break;
// 		case 4:
// 			snprintf(sample, sizeof(sample), "T2%02d.RAW", 0);
// 			break;
// 		case 5:
// 			snprintf(sample, sizeof(sample), "T3%02d.RAW", 0);
// 			break;
// 		case 6:
// 		case 7:		//TODO Handle the HH Pedal
// 			snprintf(sample, sizeof(sample), "HH%02d.RAW", 0);
// 			break;
// 		case 8:
// 			snprintf(sample, sizeof(sample), "SP%02d.RAW", 0);
// 			break;
// 		case 9:
// 			snprintf(sample, sizeof(sample), "CR%02d.RAW", 0);
// 			break;
// 		case 10:
// 			snprintf(sample, sizeof(sample), "RD%02d.RAW", 0);
// 			break;
// 		case 11:
// 			snprintf(sample, sizeof(sample), "CX%02d.RAW", 0);
// 			break;
// 	}
// 	
//	uint8_t c = findAvailableSample();
	mixer.gain(mixerIndex, volume / 2);
	sample.play("SN00.RAW");			//TODO Change to be dynamic
	
// 	_sampleToChannelMap[c] = channel;
// 	_channelToSampleMap[channel] = c;
// 	_lastPlayedChannel[channel] = millis();
// 	_lastPlayedValue[channel] = value;
	
	Serial.print("Sample: ");
	Serial.print(value);
	Serial.print(",");
	Serial.println(millis());

	
	//Serial.print("Playing sample index ");
	//Serial.print(c);
	//Serial.print(" at volume ");
	//Serial.print(volume);
	//Serial.println();
	
	//Serial.print("CPU: ");
	//Serial.print(AudioProcessorUsage());
	//Serial.print(",");
	//Serial.print(AudioProcessorUsageMax());
	//Serial.print("    ");
	//Serial.print("Memory: ");
	//Serial.print(AudioMemoryUsage());
	//Serial.print(",");
	//Serial.print(AudioMemoryUsageMax());
	//Serial.println();
}