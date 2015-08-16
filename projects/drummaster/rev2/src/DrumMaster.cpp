#include "DrumMaster.h"

using namespace digitalcave;

ADC adc;

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
	
	//Set up the ADC
	adc.setResolution(8);
	adc.setConversionSpeed(ADC_LOW_SPEED);
	adc.setAveraging(16);

	//Allocate enough memory for audio
	AudioMemory(16);
	
	//Turn on the audio chip
	Samples::setMasterVolume(0.7);	//TODO Load from EEPROM
	
	while (1){
		menuState.poll();
		
		for (uint8_t i = 0; i < 8; i++){		//TODO Change 8 to PAD_COUNT
			pads[i].poll();
		}
	}
}


