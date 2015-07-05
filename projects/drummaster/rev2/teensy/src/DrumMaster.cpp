#include <ADC.h>
#include <Audio.h>
#include <Bounce.h>
#include <SD.h>
#include <SPI.h>

//Pin assignments
#define MUX0						0
#define MUX1						1
#define MUX2						2
#define MUX3						3
#define DRAIN_EN					4
#define ADC_EN						5
#define ADC_INPUT					A14
#define ENC_PUSH					15
#define ENC_A						20
#define ENC_B						21

//Constants
#define MIN_VALUE					10
#define SAMPLE_COUNT				16

AudioPlaySdRaw samples[SAMPLE_COUNT];
AudioMixer4 mixers[4];
AudioMixer4 masterMixer;
AudioOutputI2S output;
AudioConnection patchCord00(samples[0],  0, mixers[0], 0);
AudioConnection patchCord01(samples[1],  0, mixers[0], 1);
AudioConnection patchCord02(samples[2],  0, mixers[0], 2);
AudioConnection patchCord03(samples[3],  0, mixers[0], 3);
AudioConnection patchCord04(samples[4],  0, mixers[1], 0);
AudioConnection patchCord05(samples[5],  0, mixers[1], 1);
AudioConnection patchCord06(samples[6],  0, mixers[1], 2);
AudioConnection patchCord07(samples[7],  0, mixers[1], 3);
AudioConnection patchCord08(samples[8],  0, mixers[2], 0);
AudioConnection patchCord09(samples[9],  0, mixers[2], 1);
AudioConnection patchCord10(samples[10], 0, mixers[2], 2);
AudioConnection patchCord11(samples[11], 0, mixers[2], 3);
AudioConnection patchCord12(samples[12], 0, mixers[3], 0);
AudioConnection patchCord13(samples[13], 0, mixers[3], 1);
AudioConnection patchCord14(samples[14], 0, mixers[3], 2);
AudioConnection patchCord15(samples[15], 0, mixers[3], 3);

AudioConnection patchCord16(mixers[0], 0, masterMixer, 0);
AudioConnection patchCord17(mixers[1], 0, masterMixer, 1);
AudioConnection patchCord18(mixers[2], 0, masterMixer, 2);
AudioConnection patchCord19(mixers[3], 0, masterMixer, 3);
AudioConnection patchCord20(masterMixer, 0, output, 0);
AudioConnection patchCord21(masterMixer, 0, output, 1);
AudioControlSGTL5000     sgtl5000;

ADC adc;

Bounce button0 = Bounce(ENC_PUSH, 25);

uint8_t readDrum(uint8_t channel){
	if (channel >= 8) return 0;		//TODO Remove on real board; breadboard MUX only have 8 inputs
	
	//Disable both MUXs
	digitalWriteFast(ADC_EN, 0);
	digitalWriteFast(DRAIN_EN, 0);
	
	delayMicroseconds(1);
	
	//Set channel
	digitalWriteFast(MUX0, channel & 0x01);
	digitalWriteFast(MUX1, channel & 0x02);
	digitalWriteFast(MUX2, channel & 0x04);
	digitalWriteFast(MUX3, channel & 0x08);

	delayMicroseconds(1);

	//Enable ADC MUX, read value, and disable MUX again
	digitalWriteFast(ADC_EN, 1);
	delayMicroseconds(1);
	uint8_t result = adc.analogRead(ADC_INPUT);
	digitalWriteFast(ADC_EN, 0);
	
	if (result > MIN_VALUE){
		//Reset the peak value
		digitalWriteFast(DRAIN_EN, 1);
	}
	
	return result;
}

/*
 * Find the first available sample that is not currently playing; if all samples
 * are playing, then find the oldest one to stop it.
 */
uint8_t findAvailableSample(){
	uint8_t oldestSample = 0;
	uint16_t oldestSamplePosition = 0;
	for (uint8_t i = 0; i < 4; i++){	//TODO Change 4 to SAMPLE_COUNT
		if (samples[i].isPlaying() == 0){
			return i;
		}
		else if (samples[i].positionMillis() > oldestSamplePosition){
			oldestSamplePosition = samples[i].positionMillis();
			oldestSample = i;
		}
	}
	samples[oldestSample].stop();
	return oldestSample;
}

void playSample(uint8_t channel, uint8_t value){
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
	
	uint8_t c = findAvailableSample();
	mixers[c >> 2].gain(c & 0x03, volume / 2);
	samples[c].play("00.RAW");
	
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

int main(){
	//Audio shield SPI setup
	SPI.setMOSI(7);
	SPI.setMISO(12);
	SPI.setSCK(14);

	//Allocate enough memory for audio
	AudioMemory(16);
	
	//Turn on the audio chip
	sgtl5000.enable();
	sgtl5000.volume(0.5);
	
	//Enable pins
	pinMode(MUX0, OUTPUT);
	pinMode(MUX1, OUTPUT);
	pinMode(MUX2, OUTPUT);
	pinMode(MUX3, OUTPUT);
	pinMode(DRAIN_EN, OUTPUT);
	pinMode(ADC_EN, OUTPUT);
	pinMode(ENC_PUSH, INPUT_PULLUP);	//Encoder pushbutton
	pinMode(ENC_A, INPUT_PULLUP);	//Encoder A
	pinMode(ENC_B, INPUT_PULLUP);	//Encoder B
	
	//Set up ADC
	adc.setResolution(8);

	if (!(SD.begin(10))) {
		// stop here, but print a message repetitively
		while (1) {
			Serial.println("Unable to access the SD card");
			delay(500);
		}
	}
	
	uint8_t channel = 0;
	while (1){
		button0.update();
		
		uint8_t value = readDrum(channel);
		if (value > MIN_VALUE){
			playSample(channel, value);
		}
		channel = (channel + 1) & 0x0F;
/*		
		if (button0.fallingEdge()) {
			Serial.print("Button push sample ");
			Serial.println(sampleCounter);
			samples[sampleCounter].stop();
			samples[sampleCounter].play("00.RAW");
			sampleCounter = (sampleCounter + 1) & 0x03;
			

		}
		
		delay(5);
*/
	}
}


