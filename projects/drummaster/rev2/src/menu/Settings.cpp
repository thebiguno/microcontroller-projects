#include "Settings.h"

using namespace digitalcave;

static const char* labels[] = {
	"Pads Volume         ",
	"Calibrate Channels  ",
	"Load From SD        ",
	"Load From Serial    ",
	"Reset EEPROM        ",
	"System Stats        "
};

Settings::Settings() : Menu(sizeof(labels) / sizeof(labels[0])){
}

Menu* Settings::handleAction(){
	display->write_text(0, 0, "Settings            ", 20);
	
	int8_t positionOffset = getPositionOffset();
	writeSelection(positionOffset);

	display->write_text(1, 1, labels[getMenuPosition(positionOffset - 1)], 19);
	display->write_text(2, 1, labels[getMenuPosition(positionOffset)], 19);
	display->write_text(3, 1, labels[getMenuPosition(positionOffset + 1)], 19);
	
	if (button.releaseEvent()){
		switch(getMenuPosition(0)){
			case 0:
				return Menu::volumePadSelect;
			case 1:
				return Menu::calibrateChannelSelect;
			case 2:
				return Menu::loadSamplesFromSD;
			case 3:
				return Menu::loadSamplesFromSerial;
			case 4:
				return Menu::resetEeprom;
			case 5:
				return Menu::stats;
		}
	}
	else if (button.longPressEvent()){
		return Menu::mainMenu;
	}
	
	return NULL;
}
