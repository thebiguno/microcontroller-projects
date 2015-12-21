#include "VolumeLineOut.h"
#include "../Sample.h"

using namespace digitalcave;

#define GAIN_DIVISOR		(256.0)

VolumeLineOut::VolumeLineOut() : Menu(256){
}

void VolumeLineOut::loadVolumeFromEeprom(){
	((VolumeLineOut*) Menu::volumeLineOut)->volume = EEPROM.read(EEPROM_MAIN_VOLUME);
	((VolumeLineOut*) Menu::volumeLineOut)->encoderState = ((VolumeLineOut*) Menu::volumeLineOut)->volume;
	Sample::setVolumeLineOut(((VolumeLineOut*) Menu::volumeLineOut)->volume / GAIN_DIVISOR);
}

void VolumeLineOut::saveVolumeToEeprom(){
	EEPROM.update(EEPROM_MAIN_VOLUME, ((VolumeLineOut*) Menu::volumeLineOut)->volume);
}

Menu* VolumeLineOut::handleAction(){
	display->write_text(0, 0, "Line Out Volume      ", 20);
	
	if (volume != getMenuPosition()){
		volume = getMenuPosition();
		Sample::setVolumeLineOut(volume / GAIN_DIVISOR);
	}
	snprintf(buf, sizeof(buf), "%d%%     ", (uint8_t) (volume / GAIN_DIVISOR * 100));
	display->write_text(1, 0, buf, 4);
	
	if (button.longPressEvent()){
		display->clear();
		saveVolumeToEeprom();
		return Menu::mainMenu;
	}
	
	return NULL;
}
