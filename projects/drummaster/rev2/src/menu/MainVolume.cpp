#include "MainVolume.h"
#include "../Channel.h"

using namespace digitalcave;

MainVolume::MainVolume(){
}

void MainVolume::loadVolumeFromEeprom(){
	((MainVolume*) Menu::mainVolume)->volume = EEPROM.read(EEPROM_MAIN_VOLUME);
	((MainVolume*) Menu::mainVolume)->encoderState = ((MainVolume*) Menu::mainVolume)->volume;
	Channel::setMasterVolume(((MainVolume*) Menu::mainVolume)->volume);
}

void MainVolume::saveVolumeToEeprom(){
	EEPROM.update(EEPROM_MAIN_VOLUME, ((MainVolume*) Menu::mainVolume)->volume);
}

Menu* MainVolume::handleAction(){
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
		Channel::setMasterVolume(volume);
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
