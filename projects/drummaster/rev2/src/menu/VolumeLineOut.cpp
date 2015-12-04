#include "VolumeLineOut.h"
#include "../Sample.h"

using namespace digitalcave;

VolumeLineOut::VolumeLineOut(){
}

void VolumeLineOut::loadVolumeFromEeprom(){
	((VolumeLineOut*) Menu::volumeLineOut)->volume = EEPROM.read(EEPROM_MAIN_VOLUME);
	((VolumeLineOut*) Menu::volumeLineOut)->encoderState = ((VolumeLineOut*) Menu::volumeLineOut)->volume;
	Sample::setVolumeLineOut(((VolumeLineOut*) Menu::volumeLineOut)->volume / 256.0 * 2);
}

void VolumeLineOut::saveVolumeToEeprom(){
	EEPROM.update(EEPROM_MAIN_VOLUME, ((VolumeLineOut*) Menu::volumeLineOut)->volume);
}

Menu* VolumeLineOut::handleAction(){
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
		Sample::setVolumeLineOut(volume / 256.0 * 2);
	}
	snprintf(buf, sizeof(buf), "%d%%     ", (uint8_t) (volume / 256.0 * 200));
	display->write_text(1, 0, buf, 4);
	
	if (button.fallingEdge()){
		display->write_text(1, 0, "    ", 4);
		saveVolumeToEeprom();
		return Menu::mainMenu;
	}
	
	return NULL;
}
