#include "MainMenu.h"
#include "../Sample.h"
#include "../util/Mapping.h"

using namespace digitalcave;

MainMenu::MainMenu() : Menu(4){
}

/* 
 * Display layout:
 *  01234567890123456789
 * 0    Drum  Master    
 * 1>Kit Name (19 Chars)
 * 2Vol. >xxx%    >xxx%
 * 3>Settings
 */

Menu* MainMenu::handleAction(){
	//Constant text
	display->write_text(0, 0, "    Drum  Master    ", 20);
	display->write_text(2, 0, "Vol. ", 5);
	display->write_text(2, 9, "%    ", 5);
	display->write_text(2, 18, "% ", 2);
	display->write_text(3, 1, "Settings           ", 19);

	//Dynamic text
	snprintf(buf, sizeof(buf), "%s                   ", Mapping::getMapping(Mapping::getSelectedKit())->getKitName());
	display->write_text(1, 1, buf, 19);
	snprintf(buf, sizeof(buf), "%3d", Sample::getVolumeHeadphones());
	display->write_text(2, 0, buf, 3);
	snprintf(buf, sizeof(buf), "%3d", Sample::getVolumeLineIn());
	display->write_text(2, 15, buf, 3);
	
	//Menu positioning
	if (getMenuPosition(0) == 0){
		display->write_text(1, 0, (char) 0x7E);
		display->write_text(2, 5, ' ');
		display->write_text(2, 14, ' ');
		display->write_text(3, 0, ' ');
	}
	else if (getMenuPosition(0) == 1){
		display->write_text(1, 0, ' ');
		display->write_text(2, 5, (char) 0x7E);
		display->write_text(2, 14, ' ');
		display->write_text(3, 0, ' ');
	}
	else if (getMenuPosition(0) == 2){
		display->write_text(1, 0, ' ');
		display->write_text(2, 5, ' ');
		display->write_text(2, 14, (char) 0x7E);
		display->write_text(3, 0, ' ');
	}
	else if (getMenuPosition(0) == 3){
		display->write_text(1, 0, ' ');
		display->write_text(2, 5, ' ');
		display->write_text(2, 14, ' ');
		display->write_text(3, 0, (char) 0x7E);
	}
	
	if (button.releaseEvent()){
		switch(getMenuPosition(0)){
			case 0:
				return Menu::kitSelect;
			case 1:
				return Menu::volumeHeadphones;
			case 2:
				return Menu::volumeLineIn;
			case 3:
				return Menu::settings;
		}
	}
	
	return NULL;
}
