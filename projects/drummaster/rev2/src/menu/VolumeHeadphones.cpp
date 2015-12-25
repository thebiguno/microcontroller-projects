#include "VolumeHeadphones.h"
#include "../Sample.h"

using namespace digitalcave;

#define GAIN_DIVISOR		(100.0)

VolumeHeadphones::VolumeHeadphones() : Menu(101, 0){
}

void VolumeHeadphones::loadVolumeFromEeprom(){
	uint8_t volume = EEPROM.read(EEPROM_MAIN_VOLUME);
	((VolumeHeadphones*) Menu::volumeHeadphones)->volume = volume;
	((VolumeHeadphones*) Menu::volumeHeadphones)->encoderState = volume * 2;
	Sample::setVolumeHeadphones(volume / GAIN_DIVISOR);
}

void VolumeHeadphones::saveVolumeToEeprom(){
	EEPROM.update(EEPROM_MAIN_VOLUME, ((VolumeHeadphones*) Menu::volumeHeadphones)->volume);
}

Menu* VolumeHeadphones::handleAction(){
	display->write_text(0, 0, "Headphone Volume      ", 20);
	
	if (volume != getMenuPosition(0)){
		volume = getMenuPosition(0);
		Sample::setVolumeHeadphones(volume / GAIN_DIVISOR);
	}
	snprintf(buf, sizeof(buf), "%d%%     ", (uint8_t) (volume / GAIN_DIVISOR * 100));
	display->write_text(1, 0, buf, 4);
	
	if (button.releaseEvent() || button.longPressEvent()){
		display->clear();
		saveVolumeToEeprom();
		return Menu::mainMenu;
	}
	
	return NULL;
}
