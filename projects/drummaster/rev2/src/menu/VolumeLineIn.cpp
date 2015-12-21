#include "VolumeLineIn.h"
#include "../Sample.h"

using namespace digitalcave;

#define GAIN_DIVISOR		(256.0)

VolumeLineIn::VolumeLineIn() : Menu(256){
}

void VolumeLineIn::loadVolumeFromEeprom(){
	((VolumeLineIn*) Menu::volumeLineIn)->volume = EEPROM.read(EEPROM_LINE_IN_VOLUME);
	((VolumeLineIn*) Menu::volumeLineIn)->encoderState = ((VolumeLineIn*) Menu::volumeLineIn)->volume;
	Sample::setVolumeLineIn(((VolumeLineIn*) Menu::volumeLineIn)->volume / GAIN_DIVISOR);
}

void VolumeLineIn::saveVolumeToEeprom(){
	EEPROM.update(EEPROM_LINE_IN_VOLUME, ((VolumeLineIn*) Menu::volumeLineIn)->volume);
}

Menu* VolumeLineIn::handleAction(){
	display->write_text(0, 0, "Line In Volume       ", 20);

	if (volume != getMenuPosition()){
		volume = getMenuPosition();
		Sample::setVolumeLineIn(volume / GAIN_DIVISOR);
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
