#include "KitSelect.h"

using namespace digitalcave;

KitSelect::KitSelect() : lastKitIndex(0xFF){
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
			display->write_text(1, 0, "<Main Menu>         ", 20);
			encoder.write(totalKitCount * 2);
		}
		else {
			snprintf(buf, sizeof(buf), "%s                   ", mapping.getKitName());
			display->write_text(1, 0, buf, 20);
		}
	}
	
	if (button.fallingEdge()){
		if (mapping.getKitIndex() == 0xFF){
			display->write_text(1, 0, "                    ", 20);
			return Menu::mainMenu;
		}
		else {
			
		}
	}
	return NULL;
}
