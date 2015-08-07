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


