#include "MainMenu.h"

using namespace digitalcave;

MainMenu::MainMenu(){
}

Menu* MainMenu::handleAction(){
	#define MENU_COUNT 7

	int8_t menuItem = encoder.read() / 2;
	if (menuItem < 0) encoder.write((MENU_COUNT - 1) * 2);		//Loop to end
	else if (menuItem >= MENU_COUNT) encoder.write(0);
	
	switch(menuItem){
		case 0:
			display->write_text(0, 0, "Line Out Volume     ", 20);
			if (button.fallingEdge()){
				return Menu::volumeLineOut;
			}
			break;
		case 1:
			display->write_text(0, 0, "Line In Volume      ", 20);
			if (button.fallingEdge()){
				return Menu::volumeLineIn;
			}
			break;
		case 2:
			display->write_text(0, 0, "Kit Select          ", 20);
			if (button.fallingEdge()){
				return Menu::kitSelect;
			}
			break;
		case 3:
			display->write_text(0, 0, "Pads Volume         ", 20);
			if (button.fallingEdge()){
				return Menu::volumePadSelect;
			}
			break;
		case 4:
			display->write_text(0, 0, "Calibrate Channels  ", 20);
			if (button.fallingEdge()){
				return Menu::calibrateChannelSelect;
			}
			break;
		case 5:
			display->write_text(0, 0, "Load From SD        ", 20);
			if (button.fallingEdge()){
				return Menu::loadFromSD;
			}
			break;
		case 6:
			display->write_text(0, 0, "System Stats        ", 20);
			if (button.fallingEdge()){
				return Menu::stats;
			}
			break;
	}
	
	return NULL;
}
