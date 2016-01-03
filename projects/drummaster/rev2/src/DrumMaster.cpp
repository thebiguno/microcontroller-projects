#include "DrumMaster.h"

using namespace digitalcave;

int main(){
	//Enable pins
	pinMode(MUX0, OUTPUT);
	pinMode(MUX1, OUTPUT);
	pinMode(MUX2, OUTPUT);
	pinMode(MUX3, OUTPUT);
	pinMode(DRAIN_EN, OUTPUT);
	pinMode(ADC_EN, OUTPUT);
	
	//Init the display
	Menu::initDisplay();
	
	//while(!Serial);		//Wait for a serial console before continuing.  Only needed for debugging at startup.
	Serial.begin(9600);	//Teensy serial is always at full USB speed and buffered... the baud rate here is required but ignore
	Wire.begin();
	
	//Audio shield SD / flash setup
	SPI.setMOSI(MOSI);
	SPI.setMISO(MISO);
	SPI.setSCK(SCK);
	SerialFlash.begin(CS_FLASH);
	SD.begin(CS_SD);

	//Encoder pushbutton
	pinMode(ENC_PUSH, INPUT_PULLUP);
	
	//Allocate enough memory for audio
	AudioMemory(AUDIO_MEMORY);
	
	//Load kit mappings from flash
	Mapping::loadMappings();
	
	//Load settings from EEPROM
	CalibrateChannel::loadPotentiometerFromEeprom();
	KitSelect::loadKitIndexFromEeprom();
	VolumeLineIn::loadVolumeFromEeprom();
	VolumeHeadphones::loadVolumeFromEeprom();

	//Set up ADC and build filename tables
	Pad::init();
	
	while (1){
		Menu::poll();
		
		for (uint8_t i = 0; i < PAD_COUNT; i++){
			Pad::pads[i]->poll();
		}
	}
}


