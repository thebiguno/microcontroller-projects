#include "KitSelect.h"

using namespace digitalcave;

KitSelect::KitSelect() : lastKitIndex(0xFF){
}

void KitSelect::loadKitIndexFromEeprom(){
	uint8_t kitIndex = EEPROM.read(EEPROM_KIT_INDEX);
	((KitSelect*) Menu::kitSelect)->encoderState = kitIndex * 2;
	Pad::loadAllSamples(kitIndex, ((KitSelect*) Menu::kitSelect)->getMapping());
}

void KitSelect::saveKitIndexToEeprom(){
	EEPROM.update(EEPROM_KIT_INDEX, ((KitSelect*) Menu::kitSelect)->getMapping()->getKitIndex());
}

Menu* KitSelect::handleAction(){
	int8_t kitIndex = encoder.read() / 2;
	if (kitIndex != lastKitIndex){
		lastKitIndex = kitIndex;
		
		uint8_t totalKitCount = Mapping::loadKit(kitIndex, &mapping);
	
		if (kitIndex < 0){
			encoder.write(totalKitCount * 2);
		}
		else if (kitIndex > totalKitCount){
			encoder.write(0);
		}
		else if (mapping.getKitIndex() == 0xFF){
			display->clear();
			display->write_text(1, 0, "<Main Menu>         ", 20);
			encoder.write(totalKitCount * 2);
		}
		else {
			snprintf(buf, sizeof(buf), "%s                   ", mapping.getKitName());
			display->write_text(1, 0, "Load                ", 20);
			display->write_text(2, 0, buf, 20);
		}
	}
	
	if (button.fallingEdge()){
		if (mapping.getKitIndex() == 0xFF){
			display->clear();
			lastKitIndex = 0xFF;
			return Menu::mainMenu;
		}
		else {
			VolumePad::savePadVolumesToEeprom();
			Pad::loadAllSamples(mapping.getKitIndex(), &mapping);
			saveKitIndexToEeprom();
			VolumePad::loadPadVolumesFromEeprom();
			display->clear();
			lastKitIndex = 0xFF;
			return Menu::mainMenu;
		}
	}
	return NULL;
}

Mapping* KitSelect::getMapping(){
	return &mapping;
}
