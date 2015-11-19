#include "MainVolume.h"
#include "../Sample.h"

using namespace digitalcave;

uint8_t MainVolume::volume;

MainVolume::MainVolume(){
}

void MainVolume::loadVolumeFromEeprom(){
	volume = EEPROM.read(EEPROM_MAIN_VOLUME);
	((MainVolume*) Menu::mainVolume)->encoderState = volume;
	Serial.print("Loading volume ");
	Serial.println(volume);	
	Sample::setMasterVolume(volume);
}

void MainVolume::saveVolumeToEeprom(){
	Serial.print("Saving volume ");
	Serial.println(volume);
	EEPROM.update(EEPROM_MAIN_VOLUME, volume);
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
		Sample::setMasterVolume(volume);
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
