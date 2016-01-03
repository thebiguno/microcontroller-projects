#include "Menu.h"

#include "CalibrateChannel.h"
#include "CalibrateChannelSelect.h"
#include "LoadSamplesFromSD.h"
#include "LoadSamplesFromSerial.h"
#include "KitSelect.h"
#include "MainMenu.h"
#include "ResetEeprom.h"
#include "Settings.h"
#include "Stats.h"
#include "VolumeLineIn.h"
#include "VolumeHeadphones.h"
#include "VolumePad.h"
#include "VolumePadSelect.h"

using namespace digitalcave;

//Initialize static member variables
char Menu::buf[21];
Hd44780_Teensy* Menu::hd44780 = NULL;
CharDisplay* Menu::display = NULL;
Encoder Menu::encoder(ENC_A, ENC_B);
ButtonTeensy Menu::button(ENC_PUSH, 25, 25, 500, 500);
uint32_t Menu::lastTime = 0;

//Initialize static references to menu items
Menu* Menu::calibrateChannel = new CalibrateChannel();
Menu* Menu::calibrateChannelSelect = new CalibrateChannelSelect();
Menu* Menu::loadSamplesFromSD = new LoadSamplesFromSD();
Menu* Menu::loadSamplesFromSerial = new LoadSamplesFromSerial();
Menu* Menu::kitSelect = new KitSelect();
Menu* Menu::mainMenu = new MainMenu();
Menu* Menu::resetEeprom = new ResetEeprom();
Menu* Menu::settings = new Settings();
Menu* Menu::stats = new Stats();
Menu* Menu::volumeLineIn = new VolumeLineIn();
Menu* Menu::volumeHeadphones = new VolumeHeadphones();
Menu* Menu::volumePad = new VolumePad();
Menu* Menu::volumePadSelect = new VolumePadSelect();

Menu* Menu::current = Menu::mainMenu;

Menu::Menu(uint16_t menuCount) : 
	menuCount(menuCount), 
	encoderState(0){
}

void Menu::initDisplay(){
	//Give the power some time to stabilize.  If we start too soon, the
	// display can end up in a bad state.
	delay(50);
	
	hd44780 = new Hd44780_Teensy(Menu::hd44780->FUNCTION_LINE_2 | Menu::hd44780->FUNCTION_SIZE_5x8, PIN_RS, PIN_E, MUX0, MUX1, MUX2, MUX3);
	display = new CharDisplay(Menu::hd44780, DISPLAY_ROWS, DISPLAY_COLS);
	display->write_text(0, 0, "Loading...          ", 20);
	display->refresh();
	
	{
		uint8_t custom[64] = {
			0x08,0x0C,0x1E,0x1F,0x1E,0x0C,0x08,0x00,	// 0 - ARROW_BOLD
			0x08,0x0C,0x06,0x03,0x06,0x0C,0x08,0x00,	// 1 - ARROW_NORMAL
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 2
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 3
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 4
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 5
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// 6
			0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00		// 7
		};
		hd44780->set_cgram_address(0x00);
		delay(64);
		hd44780->write_bytes(custom, 64);
	}
}

void Menu::poll(){
	if (lastTime + 100 < millis()){
		button.sample(millis());
		
		//Ensure valid menu entry is selected
		if (current->menuCount == 0) encoder.write(0);

		//Prevent overflow
		if ((encoder.read() / 2) >= current->menuCount) encoder.write((current->menuCount - 1) * 2);
		else if ((encoder.read() / 2) < 0) encoder.write(0);

		Menu* newMenu = current->handleAction();
		display->refresh();
		if (newMenu != NULL){
			change(newMenu);
		}
		
		lastTime = millis();
	}
}

void Menu::change(Menu* newMenu){
	current->encoderState = encoder.read();
	current = newMenu;
	encoder.write(newMenu->encoderState);
	display->clear();
}

void Menu::setMenuCount(uint16_t menuCount){
	this->menuCount = menuCount;
}

int16_t Menu::getMenuPosition(int8_t offset){
	int16_t value = (encoder.read() / 2) + offset;
	if (value < 0 || menuCount == 0) value = 0;
	else if (value >= menuCount) value = menuCount - 1;
	
	return value;
}

void Menu::setMenuPosition(int16_t position){
	encoder.write((int16_t) position * 2);
}

int8_t Menu::getPositionOffset(){
	if (getMenuPosition(0) == 0){
		return 1;
	}
	else if (getMenuPosition(0) == menuCount - 1){
		return -1;
	}
	return 0;
}

void Menu::writeSelection(int8_t positionOffset){
	display->write_text(1, 0, positionOffset == 1 ? ARROW_NORMAL : ' ');
	if (menuCount == 2){
		display->write_text(2, 0, positionOffset == -1 ? ARROW_NORMAL : ' ');
	}
	else if (menuCount > 2){
		display->write_text(2, 0, positionOffset == 0 ? ARROW_NORMAL : ' ');
		display->write_text(3, 0, positionOffset == -1 ? ARROW_NORMAL : ' ');
	}
}