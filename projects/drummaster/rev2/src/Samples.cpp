#include "Samples.h"

using namespace digitalcave;

Samples::Samples():
	_patchCord00(_samples[0],  0, _mixer, 0),
	_patchCord01(_samples[1],  0, _mixer, 1),
	_patchCord02(_samples[2],  0, _mixer, 2),
	_patchCord03(_samples[3],  0, _mixer, 3),
	_patchCord04(_samples[4],  0, _mixer, 4),
	_patchCord05(_samples[5],  0, _mixer, 5),
	_patchCord06(_samples[6],  0, _mixer, 6),
	_patchCord07(_samples[7],  0, _mixer, 7),
	_patchCord08(_samples[8],  0, _mixer, 8),
	_patchCord09(_samples[9],  0, _mixer, 9),
	_patchCord10(_samples[10], 0, _mixer, 10),
	_patchCord11(_samples[11], 0, _mixer, 11),
	
	_patchCord12(_input, 0, _mixer, 12),
	_patchCord13(_input, 1, _mixer, 13),

	_patchCord14(_mixer, 0, _output, 0),
	_patchCord15(_mixer, 0, _output, 1)
{	
	//Allocate enough memory for audio
	AudioMemory(16);
	
	//Initialize maps
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++){
		_sampleToChannelMap[i] = 0xFF;
	}
	for (uint8_t i = 0; i < CHANNEL_COUNT; i++){
		_channelToSampleMap[i] = 0xFF;
	}
}


uint8_t Samples::findAvailableSample(){
	uint8_t oldestSample = 0;
	uint16_t oldestSamplePosition = 0;
	for (uint8_t i = 0; i < SAMPLE_COUNT; i++){		//TODO
		if (_samples[i].isPlaying() == 0){
			return i;
		}
		else if (_samples[i].positionMillis() > oldestSamplePosition){
			oldestSamplePosition = _samples[i].positionMillis();
			oldestSample = i;
		}
	}
	_samples[oldestSample].stop();
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
		case 7:		//TODO Handle the HH Pedal
			snprintf(sample, sizeof(sample), "HH%02d.RAW", 0);
			break;
		case 8:
			snprintf(sample, sizeof(sample), "SP%02d.RAW", 0);
			break;
		case 9:
			snprintf(sample, sizeof(sample), "CR%02d.RAW", 0);
			break;
		case 10:
			snprintf(sample, sizeof(sample), "RD%02d.RAW", 0);
			break;
		case 11:
			snprintf(sample, sizeof(sample), "CX%02d.RAW", 0);
			break;
	}
	
	//Double trigger detection
	if (_lastPlayedChannel[channel] + DOUBLE_HIT_THRESHOLD > millis()){
		if (_lastPlayedValue[channel] < value){
			//Change the last volume to the higher (new) value and stop processing
			_mixer.gain(_channelToSampleMap[channel], volume / 2);
			//TODO Change the sample + volume to match the highest value
			Serial.println("Adjusted last hit");
			_lastPlayedChannel[channel] = millis();
			return;
		}
		else {
			Serial.println("Ignoring double trigger");
			return;
		}
	}
// 	else {
// 		_lastPlayedChannel[channel] = millis();
// 		//Change the last volume to the higher (new) value and return
// 		_mixer.gain(_channelToSampleMap[channel], volume / 2);
// 		//TODO Change the sample + volume to match the highest value
// 		return;
// 	}

	uint8_t c = findAvailableSample();
	_mixer.gain(c, volume / 2);
	_samples[c].play("SN00.RAW");			//TODO Change to be dynamic
	
	_sampleToChannelMap[c] = channel;
	_channelToSampleMap[channel] = c;
	_lastPlayedChannel[channel] = millis();
	_lastPlayedValue[channel] = value;
	
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