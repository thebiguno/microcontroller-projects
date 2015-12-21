#include "VolumeLineOut.h"
#include "../Sample.h"

using namespace digitalcave;

#define GAIN_DIVISOR		(100.0)

VolumeLineOut::VolumeLineOut() : Menu(101, 0){
}

void VolumeLineOut::loadVolumeFromEeprom(){
	uint8_t volume = EEPROM.read(EEPROM_MAIN_VOLUME);
	((VolumeLineOut*) Menu::volumeLineOut)->volume = volume;
	((VolumeLineOut*) Menu::volumeLineOut)->encoderState = volume * 2;
	Sample::setVolumeLineOut(volume / GAIN_DIVISOR);
}

void VolumeLineOut::saveVolumeToEeprom(){
	EEPROM.update(EEPROM_MAIN_VOLUME, ((VolumeLineOut*) Menu::volumeLineOut)->volume);
}

Menu* VolumeLineOut::handleAction(){
	display->write_text(0, 0, "Line Out Volume      ", 20);
	
	if (volume != getMenuPosition(0)){
		volume = getMenuPosition(0);
		Sample::setVolumeLineOut(volume / GAIN_DIVISOR);
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
