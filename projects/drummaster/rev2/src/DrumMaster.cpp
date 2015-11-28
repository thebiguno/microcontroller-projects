#include "DrumMaster.h"

using namespace digitalcave;

int main(){
	//Give the power some time to stabilize.  If we start too soon, the
	// display can end up in a bad state.
	delay(200);
	
	//while(!Serial);		//Wait for a serial console before continuing.  Only needed for debugging at startup.
	Serial.begin(9600);
	Wire.begin();
	
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
	
	Pad::pads[0] = new HiHat(0, 1);	//Hihat + Pedal
	Pad::pads[1] = new Drum(2);		//Snare
	Pad::pads[2] = new Drum(3);		//Bass
	Pad::pads[3] = new Drum(4);		//Tom1
	Pad::pads[4] = new Cymbal(5);	//Crash
	Pad::pads[5] = new Drum(6);		//Tom2
	Pad::pads[6] = new Drum(7);		//Tom3
	Pad::pads[7] = new Cymbal(8);	//Splash
	Pad::pads[8] = new Cymbal(9);	//Ride
	Pad::pads[9] = new Drum(10);	//X0
	Pad::pads[10] = new Drum(11);	//X1
	
	//Load settings from EEPROM
	MainVolume::loadVolumeFromEeprom();
	LineInVolume::loadVolumeFromEeprom();
	CalibratePad::loadPotentiometerFromEeprom();
	
	while (1){
		Menu::poll();
		
		for (uint8_t i = 0; i < PAD_COUNT; i++){
			Pad::pads[i]->poll();
		}
	}
}


