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

void Menu::poll(){
	if (lastTime + 50 < millis()){
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
	display->write_text(1, 0, positionOffset == 1 ? (char) 0x7E : ' ');
	if (menuCount == 2){
		display->write_text(2, 0, positionOffset == -1 ? (char) 0x7E : ' ');
	}
	else if (menuCount > 2){
		display->write_text(2, 0, positionOffset == 0 ? (char) 0x7E : ' ');
		display->write_text(3, 0, positionOffset == -1 ? (char) 0x7E : ' ');
	}
}