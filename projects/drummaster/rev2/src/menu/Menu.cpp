#include "Menu.h"

#include "CalibrateChannel.h"
#include "CalibrateChannelSelect.h"
#include "LoadMappingsFromSD.h"
#include "LoadSamplesFromSD.h"
#include "KitSelect.h"
#include "MainMenu.h"
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

//Initialize static references to menu items
Menu* Menu::calibrateChannel = new CalibrateChannel();
Menu* Menu::calibrateChannelSelect = new CalibrateChannelSelect();
Menu* Menu::loadMappingsFromSD = new LoadMappingsFromSD();
Menu* Menu::loadSamplesFromSD = new LoadSamplesFromSD();
Menu* Menu::kitSelect = new KitSelect();
Menu* Menu::mainMenu = new MainMenu();
Menu* Menu::stats = new Stats();
Menu* Menu::volumeLineIn = new VolumeLineIn();
Menu* Menu::volumeHeadphones = new VolumeHeadphones();
Menu* Menu::volumePad = new VolumePad();
Menu* Menu::volumePadSelect = new VolumePadSelect();

Menu* Menu::current = Menu::mainMenu;

Menu::Menu(uint16_t menuCount, uint8_t loop) : 
	menuCount(menuCount), 
	loop(loop),
	encoderState(0){
}

void Menu::poll(){
	current->button.sample(millis());
	
	//Ensure valid menu entry is selected
	if (current->menuCount == 0) encoder.write(0);
	if (current->loop){
		if ((encoder.read() / 2) >= current->menuCount) encoder.write(0);
		else if ((encoder.read() / 2) < 0) encoder.write((current->menuCount - 1) * 2);
	}
	else {
		if ((encoder.read() / 2) >= current->menuCount) encoder.write((current->menuCount - 1) * 2);
		else if ((encoder.read() / 2) < 0) encoder.write(0);
	}

	Menu* newMenu = current->handleAction();
	display->refresh();
	if (newMenu != NULL){
		change(newMenu);
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
	if (loop){
		while (value < 0) {
			value += menuCount;
		}
		
		if (menuCount > 0) value = value % menuCount;
		else value = 0;
	}
	else {
		if (value < 0 || menuCount == 0) value = 0;
		else if (value >= menuCount) value = menuCount - 1;
	}
	
	return value;
}

void Menu::setMenuPosition(int16_t position){
	encoder.write((int16_t) position * 2);
}

