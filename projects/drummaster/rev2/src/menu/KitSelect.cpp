#include "KitSelect.h"

using namespace digitalcave;

KitSelect::KitSelect() : Menu(0){
}

void KitSelect::loadKitIndexFromEeprom(){
	uint8_t kitIndex = EEPROM.read(EEPROM_KIT_INDEX);
	if (kitIndex > Mapping::getKitCount()){
		kitIndex = 0;
		EEPROM.update(EEPROM_KIT_INDEX, kitIndex);
	}
	((KitSelect*) Menu::kitSelect)->encoderState = kitIndex * 2;
	((KitSelect*) Menu::kitSelect)->kitIndex = kitIndex;
	((KitSelect*) Menu::kitSelect)->setMenuCount(Mapping::getKitCount());
	Pad::loadAllSamples(kitIndex);
	VolumePad::loadPadVolumesFromEeprom(kitIndex);
}

void KitSelect::saveKitIndexToEeprom(){
	EEPROM.update(EEPROM_KIT_INDEX, ((KitSelect*) Menu::kitSelect)->kitIndex);
}

Menu* KitSelect::handleAction(){
	display->write_text(0, 0, "Select Kit Mapping   ", 20);	

	int8_t positionOffset = getPositionOffset();
	writeSelection(positionOffset);

	snprintf(buf, sizeof(buf), "%s                   ", Mapping::getMapping(getMenuPosition(positionOffset - 1))->getKitName());
	display->write_text(1, 1, buf, 19);
	snprintf(buf, sizeof(buf), "%s                   ", Mapping::getMapping(getMenuPosition(positionOffset))->getKitName());
	display->write_text(2, 1, buf, 19);
	snprintf(buf, sizeof(buf), "%s                   ", Mapping::getMapping(getMenuPosition(positionOffset + 1))->getKitName());
	display->write_text(3, 1, buf, 19);
	
	if (kitIndex != getMenuPosition(0)){
		kitIndex = getMenuPosition(0);
		Pad::loadAllSamples(kitIndex);
		VolumePad::loadPadVolumesFromEeprom(kitIndex);
	}

	if (button.releaseEvent()){
		saveKitIndexToEeprom();
		return Menu::mainMenu;
	}
	else if (button.longPressEvent()){
		loadKitIndexFromEeprom();
		setMenuPosition(kitIndex);
		return Menu::mainMenu;
	}

	return NULL;
}
