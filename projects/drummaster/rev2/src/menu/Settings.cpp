#include "Settings.h"

using namespace digitalcave;

static const char* labels[] = {
	"Pads Volume         ",
	"Calibrate Channels  ",
	"Load Kits           ",
	"Load Samples + Kits ",
	"System Stats        "
};

Settings::Settings() : Menu(sizeof(labels)){
}

Menu* Settings::handleAction(){
	display->write_text(0, 0, "Drum Master  rev 2.0", 20);
	
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
				return Menu::loadMappingsFromSD;
			case 3:
				return Menu::loadSamplesFromSD;
			case 4:
				return Menu::stats;
		}
	}
	
	return NULL;
}
