#include "DrumMaster.h"

using namespace digitalcave;

int main(){
	//Give the power some time to stabilize.  If we start too soon, the
	// display can end up in a bad state.
	delay(100);
	
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
	
	//Init the display
	Menu::hd44780 = new Hd44780_Teensy(Menu::hd44780->FUNCTION_LINE_2 | Menu::hd44780->FUNCTION_SIZE_5x8, PIN_RS, PIN_E, MUX0, MUX1, MUX2, MUX3);
	Menu::display = new CharDisplay(Menu::hd44780, DISPLAY_ROWS, DISPLAY_COLS);
	
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

	//Set up ADC and build filename tables
	Pad::init();
	
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


