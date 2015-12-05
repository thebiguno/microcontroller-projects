#include "VolumeLineIn.h"
#include "../Sample.h"

using namespace digitalcave;

#define GAIN_DIVISOR		(256.0)

VolumeLineIn::VolumeLineIn(){
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
	int16_t encoderVolume = encoder.read();
	if (volume != encoderVolume){
		if (encoderVolume > 255){
			encoderVolume = 255;
			encoder.write(255);
		}
		else if (encoderVolume < 0){
			encoderVolume = 0;
			encoder.write(0);
		}
		volume = encoderVolume;
		Sample::setVolumeLineIn(volume / GAIN_DIVISOR);
	}
	snprintf(buf, sizeof(buf), "%d%%     ", (uint8_t) (volume / GAIN_DIVISOR * 100));
	display->write_text(1, 0, buf, 4);
	
	if (button.fallingEdge()){
		display->write_text(1, 0, "    ", 4);
		saveVolumeToEeprom();
		return Menu::mainMenu;
	}
	
	return NULL;
}
