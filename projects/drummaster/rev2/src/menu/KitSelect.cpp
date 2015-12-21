#include "KitSelect.h"

using namespace digitalcave;

KitSelect::KitSelect() : Menu(Mapping::getKitCount()){
}

void KitSelect::loadKitIndexFromEeprom(){
	uint8_t kitIndex = EEPROM.read(EEPROM_KIT_INDEX);
	((KitSelect*) Menu::kitSelect)->encoderState = kitIndex * 2;
	Mapping::loadMappings();
	Pad::loadAllSamples(kitIndex);
}

void KitSelect::saveKitIndexToEeprom(){
	EEPROM.update(EEPROM_KIT_INDEX, ((KitSelect*) Menu::kitSelect)->kitIndex);
}

Menu* KitSelect::handleAction(){
	display->write_text(0, 0, "Select Kit Mapping   ", 20);	
	
	display->write_text(2, 0, (char) 0x7E);
	snprintf(buf, sizeof(buf), "%s                   ", Mapping::getMappings()[getMenuPosition(-1)].getKitName());
	display->write_text(1, 1, buf, 19);
	snprintf(buf, sizeof(buf), "%s                   ", Mapping::getMappings()[getMenuPosition()].getKitName());
	display->write_text(2, 1, buf, 19);
	snprintf(buf, sizeof(buf), "%s                   ", Mapping::getMappings()[getMenuPosition(1)].getKitName());
	display->write_text(3, 1, buf, 19);

	if (button.releaseEvent()){
		VolumePad::savePadVolumesToEeprom();
		Pad::loadAllSamples(kitIndex);
		saveKitIndexToEeprom();
		VolumePad::loadPadVolumesFromEeprom();
		display->clear();
		return Menu::mainMenu;
	}
	else if (button.longPressEvent()){
		display->clear();
		return Menu::mainMenu;
	}

	return NULL;
}
