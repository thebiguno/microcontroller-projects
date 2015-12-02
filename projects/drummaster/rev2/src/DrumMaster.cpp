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
	
	Serial.println("Foo Bar");
	
	Pad::init();
	
// 	Pad::pads[0] = new HiHat("HH", 0, 1, 15, 50);	//Hihat + Pedal
// 	Pad::pads[1] = new Drum("SN", 2, 25);		//Snare
// 	Pad::pads[2] = new Drum("BS", 3, 100);	//Bass
// 	Pad::pads[3] = new Drum("T1", 4, 50);		//Tom1
// 	Pad::pads[4] = new Cymbal("CR", 5, 14, 50);	//Crash
// 	Pad::pads[5] = new Drum("T2", 6, 50);		//Tom2
// 	Pad::pads[6] = new Drum("T3", 7, 50);		//Tom3
// 	Pad::pads[7] = new Cymbal("SP", 8, 13, 50);	//Splash
// 	Pad::pads[8] = new Cymbal("RD", 9, 12, 50);	//Ride
// 	Pad::pads[9] = new Drum("X0", 10, 50);	//X0
// 	Pad::pads[10] = new Drum("X1", 11, 50);	//X1
	
	//Load settings from EEPROM
	CalibrateChannel::loadPotentiometerFromEeprom();
	VolumeLineIn::loadVolumeFromEeprom();
	VolumeLineOut::loadVolumeFromEeprom();
	VolumePad::loadPadVolumesFromEeprom();
	
	while (1){
		Menu::poll();
		
		for (uint8_t i = 0; i < PAD_COUNT; i++){
			Pad::pads[i]->poll();
		}
	}
}


