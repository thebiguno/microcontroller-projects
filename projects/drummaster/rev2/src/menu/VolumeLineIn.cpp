#include "VolumeLineIn.h"
#include "../Channel.h"

using namespace digitalcave;

VolumeLineIn::VolumeLineIn(){
}

void VolumeLineIn::loadVolumeFromEeprom(){
	((VolumeLineIn*) Menu::volumeLineIn)->volume = EEPROM.read(EEPROM_LINE_IN_VOLUME);
	((VolumeLineIn*) Menu::volumeLineIn)->encoderState = ((VolumeLineIn*) Menu::volumeLineIn)->volume;
	Channel::setVolumeLineIn(((VolumeLineIn*) Menu::volumeLineIn)->volume);
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
		Channel::setVolumeLineIn(volume);
	}
	snprintf(buf, sizeof(buf), "%d%%     ", (uint8_t) (volume / 256.0 * 100));
	display.write_text(1, 0, buf, 4);
	
	if (button.fallingEdge()){
		display.write_text(1, 0, "    ", 4);
		saveVolumeToEeprom();
		return Menu::mainMenu;
	}
	
	return NULL;
}
