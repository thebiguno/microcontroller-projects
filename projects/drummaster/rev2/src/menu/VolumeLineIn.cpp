#include "VolumeLineIn.h"
#include "../Sample.h"
#include "../util/Mapping.h"

using namespace digitalcave;

VolumeLineIn::VolumeLineIn() : Menu(201){
}

void VolumeLineIn::loadVolumeFromEeprom(){
	uint8_t volume = EEPROM.read(EEPROM_LINE_IN_VOLUME);
	((VolumeLineIn*) Menu::volumeLineIn)->encoderState = volume * 2;
	Sample::setVolumeLineIn(volume);
}

void VolumeLineIn::saveVolumeToEeprom(){
	EEPROM.update(EEPROM_LINE_IN_VOLUME, Sample::getVolumeLineIn());
}

Menu* VolumeLineIn::handleAction(){
	//Change value
	if (Sample::getVolumeLineIn() != getMenuPosition(0)){
		Sample::setVolumeLineIn(getMenuPosition(0));
	}
	
	//Constant text
	display->write_text(0, 0, "    Drum  Master    ", 20);
	display->write_text(2, 0, "Volume: ", 8);
	display->write_text(2, 12, "% ", 2);
	display->write_text(2, 18, "% ", 2);
	display->write_text(3, 1, "Settings           ", 19);
	display->write_text(2, 14, ARROW_BOLD);

	//Dynamic text
	snprintf(buf, sizeof(buf), "%s                   ", Mapping::getSelectedMapping()->getKitName());
	display->write_text(1, 1, buf, 19);
	snprintf(buf, sizeof(buf), "%3d", Sample::getVolumeHeadphones());
	display->write_text(2, 9, buf, 3);
	snprintf(buf, sizeof(buf), "%3d", Sample::getVolumeLineIn());
	display->write_text(2, 15, buf, 3);

	//Change menus	
	if (button.releaseEvent() || button.longPressEvent()){
		display->clear();
		saveVolumeToEeprom();
		return Menu::mainMenu;
	}
	
	return NULL;
}
