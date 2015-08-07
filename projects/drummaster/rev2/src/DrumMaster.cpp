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

Sample samples[SAMPLE_COUNT] = {
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
};

AudioControlSGTL5000 control;

MainMenu* mainMenu;
MainVolume* mainVolume;
LoadSamples* loadSamples;
CalibratePads* calibratePads;
CalibratePad* calibratePad;

MenuState* menuState;

// uint8_t readDrum(uint8_t channel){
// 	if (channel >= 8) return 0;		//TODO Remove on real board; breadboard MUX only have 8 inputs
// 	
// 	//Disable both MUXs
// 	digitalWriteFast(ADC_EN, 0);
// 	digitalWriteFast(DRAIN_EN, 0);
// 	
// 	delayMicroseconds(1);
// 	
// 	//Set channel
// 	digitalWriteFast(MUX0, channel & 0x01);
// 	digitalWriteFast(MUX1, channel & 0x02);
// 	digitalWriteFast(MUX2, channel & 0x04);
// 	digitalWriteFast(MUX3, channel & 0x08);
// 
// 	delayMicroseconds(1);
// 
// 	//Enable ADC MUX, read value, and disable MUX again
// 	digitalWriteFast(ADC_EN, 1);
// 	delayMicroseconds(10);
// 	uint8_t result = adc.analogRead(ADC_INPUT);
// 	digitalWriteFast(ADC_EN, 0);
// 	
// 	if (result > MIN_VALUE){
// 		//Reset the peak value
// 		digitalWriteFast(DRAIN_EN, 1);
// 		delay(1);
// 	}
// 	
// 	return result;
// }

int main(){
	//Turn on the audio chip
	control.enable();
	control.volume(0.5);
	
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
	
	mainMenu = new MainMenu();
	mainVolume = new MainVolume();
	loadSamples = new LoadSamples();
	calibratePads = new CalibratePads();
	calibratePad = new CalibratePad();

	menuState = new MenuState(mainMenu);
	
	AudioMemory(16);
	
	//Turn on the audio chip
	control.enable();
	control.volume(0.8);	//TODO Load from EEPROM
	
	while (1){
		menuState->poll();
		
		for (uint8_t i = 0; i < PAD_COUNT; i++){
			pads[i].poll();
		}
	}
}


