#include "VolumeLineIn.h"
#include "../Sample.h"

using namespace digitalcave;

#define GAIN_DIVISOR		(100.0)

VolumeLineIn::VolumeLineIn() : Menu(101, 0){
}

void VolumeLineIn::loadVolumeFromEeprom(){
	uint8_t volume = EEPROM.read(EEPROM_LINE_IN_VOLUME);
	((VolumeLineIn*) Menu::volumeLineIn)->volume = volume;
	((VolumeLineIn*) Menu::volumeLineIn)->encoderState = volume * 2;
	Sample::setVolumeLineIn(volume / GAIN_DIVISOR);
}

void VolumeLineIn::saveVolumeToEeprom(){
	EEPROM.update(EEPROM_LINE_IN_VOLUME, ((VolumeLineIn*) Menu::volumeLineIn)->volume);
}

Menu* VolumeLineIn::handleAction(){
	display->write_text(0, 0, "Line In Volume       ", 20);

	if (volume != getMenuPosition(0)){
		volume = getMenuPosition(0);
		Sample::setVolumeLineIn(volume / GAIN_DIVISOR);
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
