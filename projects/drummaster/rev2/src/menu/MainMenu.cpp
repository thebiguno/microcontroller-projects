#include "MainMenu.h"

using namespace digitalcave;

#define MENU_COUNT 8
static const char* labels[MENU_COUNT] = {
	"Line Out Volume     ",
	"Line In Volume      ",
	"Kit Select          ",
	"Pads Volume         ",
	"Calibrate Channels  ",
	"Load Kit Mappings   ",
	"Load Samples        ",
	"System Stats        "
};

MainMenu::MainMenu() : Menu(MENU_COUNT, 1){
}

Menu* MainMenu::handleAction(){
	display->write_text(0, 0, "Drum Master  rev 2.0", 20);
	
	display->write_text(2, 0, (char) 0x7E);
	display->write_text(1, 1, labels[getMenuPosition(-1)], 19);
	display->write_text(2, 1, labels[getMenuPosition(0)], 19);
	display->write_text(3, 1, labels[getMenuPosition(1)], 19);
	
	if (button.releaseEvent()){
		switch(getMenuPosition(0)){
			case 0:
				return Menu::volumeLineOut;
			case 1:
				return Menu::volumeLineIn;
			case 2:
				return Menu::kitSelect;
			case 3:
				return Menu::volumePadSelect;
			case 4:
				return Menu::calibrateChannelSelect;
			case 5:
				return Menu::loadMappingsFromSD;
			case 6:
				return Menu::loadSamplesFromSD;
			case 7:
				return Menu::stats;
		}
	}
	
	return NULL;
}
