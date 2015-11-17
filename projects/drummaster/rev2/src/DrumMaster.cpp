#include "DrumMaster.h"

using namespace digitalcave;

ADC adc;

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
		
	Pad* pads[PAD_COUNT];
	pads[0] = new HiHat(0, 1);	//Hihat + Pedal
	pads[1] = new Drum(2);		//Snare
	pads[2] = new Drum(3);		//Bass
	pads[3] = new Drum(4);		//Tom1
	pads[4] = new Cymbal(5);	//Crash
	pads[5] = new Drum(6);		//Tom2
	pads[6] = new Drum(7);		//Tom3
	pads[7] = new Cymbal(8);	//Splash
	pads[8] = new Cymbal(9);	//Ride
	pads[9] = new Drum(10);		//X0
	pads[10] = new Drum(11);	//X1
	
	//Turn on the audio chip
	Sample::setMasterVolume(180);	//TODO Load from EEPROM
	
	Serial.begin(9600);
	
	while (1){
		Menu::poll();
		
		for (uint8_t i = 0; i < PAD_COUNT; i++){
			pads[i]->poll();
		}
	}
}


