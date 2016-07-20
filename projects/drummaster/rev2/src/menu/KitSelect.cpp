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
	((KitSelect*) Menu::kitSelect)->setMenuCount(Mapping::getKitCount());
	Mapping::setSelectedKit(kitIndex);
	VolumePad::loadPadVolumesFromEeprom(kitIndex);
}

void KitSelect::saveKitIndexToEeprom(){
	EEPROM.update(EEPROM_KIT_INDEX, Mapping::getSelectedKit());
}

Menu* KitSelect::handleAction(){
	//Change value
	if (Mapping::getSelectedKit() != getMenuPosition(0)){
		Mapping::setSelectedKit(getMenuPosition(0));
		VolumePad::loadPadVolumesFromEeprom(Mapping::getSelectedKit());
	}
	
	//Constant text
	display->write_text(0, 0, "    Drum  Master    ", 20);
	display->write_text(2, 0, "Volume: ", 8);
	display->write_text(2, 12, "% ", 2);
	display->write_text(2, 18, "% ", 2);
	display->write_text(3, 1, "Settings           ", 19);
	display->write_text(1, 0, ARROW_BOLD);

	//Dynamic text
	snprintf(buf, sizeof(buf), "%s                   ", Mapping::getSelectedMapping()->getKitName());
	display->write_text(1, 1, buf, 19);
	snprintf(buf, sizeof(buf), "%3d", Sample::getVolumeHeadphones());
	display->write_text(2, 9, buf, 3);
	snprintf(buf, sizeof(buf), "%3d", Sample::getVolumeLineIn());
	display->write_text(2, 15, buf, 3);

	//Change menus
	if (button.releaseEvent()){
		saveKitIndexToEeprom();
		return Menu::mainMenu;
	}
	else if (button.longPressEvent()){
		loadKitIndexFromEeprom();
		setMenuPosition(Mapping::getSelectedKit());
		return Menu::mainMenu;
	}

	return NULL;
}
