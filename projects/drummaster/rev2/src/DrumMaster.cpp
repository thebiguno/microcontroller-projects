#include "DrumMaster.h"

using namespace digitalcave;

int main(){
	//Give the power some time to stabilize.  If we start too soon, the
	// display can end up in a bad state.
	delay(50);
	
	//Enable pins
	pinMode(MUX0, OUTPUT);
	pinMode(MUX1, OUTPUT);
	pinMode(MUX2, OUTPUT);
	pinMode(MUX3, OUTPUT);
	pinMode(DRAIN_EN, OUTPUT);
	pinMode(ADC_EN, OUTPUT);
	
	//Init the display
	Menu::hd44780 = new Hd44780_Teensy(Menu::hd44780->FUNCTION_LINE_2 | Menu::hd44780->FUNCTION_SIZE_5x8, PIN_RS, PIN_E, MUX0, MUX1, MUX2, MUX3);
	Menu::display = new CharDisplay(Menu::hd44780, DISPLAY_ROWS, DISPLAY_COLS);
	Menu::display->write_text(0, 0, "Loading...          ", 20);
	Menu::display->refresh();
	
	//while(!Serial);		//Wait for a serial console before continuing.  Only needed for debugging at startup.
	Serial.begin(9600);
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

	//Load settings from EEPROM
	CalibrateChannel::loadPotentiometerFromEeprom();
	KitSelect::loadKitIndexFromEeprom();
	VolumeLineIn::loadVolumeFromEeprom();
	VolumePad::loadPadVolumesFromEeprom();
	VolumeLineOut::loadVolumeFromEeprom();
	
	//Set up ADC and build filename tables
	Pad::init();
	
	while (1){
		Menu::poll();
		
		for (uint8_t i = 0; i < PAD_COUNT; i++){
			Pad::pads[i]->poll();
		}
	}
}


