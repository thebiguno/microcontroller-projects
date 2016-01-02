#include "ResetEeprom.h"

using namespace digitalcave;

static const char* labels[] = {
	"Cancel             ",
	"Start              "
};

ResetEeprom::ResetEeprom() : Menu(2){
}

Menu* ResetEeprom::handleAction(){
	display->write_text(0, 0, "Reset EEPROM         ", 20);
	
	int8_t positionOffset = getPositionOffset();
	writeSelection(positionOffset);

	display->write_text(1, 1, labels[getMenuPosition(positionOffset - 1)], 19);
	display->write_text(2, 1, labels[getMenuPosition(positionOffset + 1)], 19);

	if (button.longPressEvent() || (button.releaseEvent() && getMenuPosition(0) == 0)){
		return Menu::settings;
	}
	else if (button.releaseEvent() && getMenuPosition(0) == 1){
		//Reset pot calibration to 100
		uint16_t values[CHANNEL_COUNT];
		for (uint8_t i = 0; i < CHANNEL_COUNT; i++){
			values[i] = 100;
		}
		EEPROM.put(EEPROM_POTENTIOMETER, values);
		
		//Headphone default volume is 70
		EEPROM.update(EEPROM_HEADPHONE_VOLUME, 70);
		
		//Line in default volume is 0
		EEPROM.update(EEPROM_LINE_IN_VOLUME, 0);
		
		//Default kit index is 0
		EEPROM.update(EEPROM_KIT_INDEX, 0);
		
		//All the pad volumes are set to 20 (100%).  We assume there are at least 20 kits, and write 
		// all values from EEPROM_PAD_VOLUME.
		for (uint8_t i = 0; i < 20; i++){
			for (uint8_t j = 0; j < PAD_COUNT; j++){
				EEPROM.update(EEPROM_PAD_VOLUME + j + (PAD_COUNT * i), 20);
			}
		}
		
		//Load settings from EEPROM
		CalibrateChannel::loadPotentiometerFromEeprom();
		KitSelect::loadKitIndexFromEeprom();
		VolumeLineIn::loadVolumeFromEeprom();
		VolumeHeadphones::loadVolumeFromEeprom();
		
		return Menu::settings;
	}
		
	return NULL;
}
