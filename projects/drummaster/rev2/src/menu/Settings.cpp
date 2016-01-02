#include "Settings.h"

using namespace digitalcave;

static const char* labels[] = {
	"Headphones Volume   ",
	"Line In Volume      ",
	"Kit Select          ",
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
				return Menu::volumeHeadphones;
			case 1:
				return Menu::volumeLineIn;
			case 2:
				return Menu::kitSelect;
			case 3:
				return Menu::volumePadSelect;
			case 4:
				return Menu::calibrateChannelSelect;
			case 5:
				return Menu::loadSamplesFromSD;
			case 6:
				return Menu::loadSamplesFromSerial;
			case 7:
				return Menu::resetEeprom;
			case 8:
				return Menu::stats;
		}
	}
	
	return NULL;
}
