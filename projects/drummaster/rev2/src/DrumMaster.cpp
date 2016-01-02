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
	
	{
		uint8_t custom[64] = {
			0x18,0x0c,0x06,0x1f,0x1f,0x06,0x0c,0x18,	// 0 - Bold right arrow
			0x0E,0x11,0x11,0x11,0x1B,0x1B,0x1B,0x00,	// 1 - Headphones
			0x0E,0x13,0x15,0x1F,0x15,0x13,0x0E,0x00,	// 2 - Line In
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 3
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 4
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 5
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 6
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00		// 7
		};
		Menu::hd44780->set_cgram_address(0x00);
		delay(64);
		Menu::hd44780->write_bytes(custom, 64);
	}
	
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


