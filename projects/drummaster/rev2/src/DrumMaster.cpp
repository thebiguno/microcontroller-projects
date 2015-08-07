#include "DrumMaster.h"

using namespace digitalcave;

Pad pads[PAD_COUNT] = {
	Pad(0),
	Pad(1),
	Pad(2),
	Pad(3),
	Pad(4),
	Pad(5),
	Pad(6),
	Pad(7),
	Pad(8),
	Pad(9),
	Pad(10)	//TODO This is HiHat
};

//Inputs
AudioPlaySerialRaw samples[SAMPLE_COUNT];
AudioInputI2S input;

//Mixer
AudioMixer16 mixer;

//Output
AudioOutputI2S output;

//Samples to mixer
AudioConnection sample0ToMixer(samples[0], 0, mixer, 0);
AudioConnection sample1ToMixer(samples[1], 0, mixer, 1);
AudioConnection sample2ToMixer(samples[2], 0, mixer, 2);
AudioConnection sample3ToMixer(samples[3], 0, mixer, 3);
AudioConnection sample4ToMixer(samples[4], 0, mixer, 4);
AudioConnection sample5ToMixer(samples[5], 0, mixer, 5);
AudioConnection sample6ToMixer(samples[6], 0, mixer, 6);
AudioConnection sample7ToMixer(samples[7], 0, mixer, 7);
AudioConnection sample8ToMixer(samples[8], 0, mixer, 8);
AudioConnection sample9ToMixer(samples[9], 0, mixer, 9);
AudioConnection sample10ToMixer(samples[10], 0, mixer, 10);
AudioConnection sample11ToMixer(samples[11], 0, mixer, 11);

//Input passthrough to mixer
AudioConnection inputToMixer0(input, 0, mixer, 12);
AudioConnection inputToMixer1(input, 1, mixer, 13);

//Mixer to output
AudioConnection mixerToOutput0(mixer, 0, output, 0);
AudioConnection mixerToOutput1(mixer, 0, output, 1);

AudioControlSGTL5000 control;

MainMenu mainMenu;
MainVolume mainVolume;
LoadSamples loadSamples;
CalibratePads calibratePads;
CalibratePad calibratePad;

MenuState menuState(&mainMenu);

int main(){
	//Enable pins
	pinMode(MUX0, OUTPUT);
	pinMode(MUX1, OUTPUT);
	pinMode(MUX2, OUTPUT);
	pinMode(MUX3, OUTPUT);
	pinMode(DRAIN_EN, OUTPUT);
	pinMode(ADC_EN, OUTPUT);
	
	//Encoder pushbutton
	pinMode(ENC_PUSH, INPUT_PULLUP);
	
	//Audio shield SD / flash setup
	SPI.setMOSI(MOSI);
	SPI.setMISO(MISO);
	SPI.setSCK(SCK);
	SerialFlash.begin(CS_FLASH);
	SD.begin(CS_SD);

	//Allocate enough memory for audio
	AudioMemory(16);
	
	//Turn on the audio chip
	control.enable();
	control.volume(0.8);	//TODO Load from EEPROM
	
	while (1){
		menuState.poll();
		
		for (uint8_t i = 0; i < PAD_COUNT; i++){
			pads[i].poll();
		}
	}
}


