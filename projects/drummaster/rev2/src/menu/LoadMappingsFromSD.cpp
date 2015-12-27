#include "LoadMappingsFromSD.h"

using namespace digitalcave;

LoadMappingsFromSD::LoadMappingsFromSD() : Menu(2){
}

Menu* LoadMappingsFromSD::handleAction(){
	display->write_text(0, 0, "Load Mappings        ", 20);
	
	if (getMenuPosition(0) == 0){
		display->write_text(1, 0, ' ');
		display->write_text(2, 0, (char) 0x7E);
	}
	else {
		display->write_text(1, 0, (char) 0x7E);
		display->write_text(2, 0, ' ');
	}

	display->write_text(1, 1, "Cancel              ", 19);
	display->write_text(2, 1, "Start               ", 19);

	if (button.releaseEvent() && getMenuPosition(0) == 1){
		display->write_text(1, 0, "Erasing Mappings.txt ", 20);
		display->clearRow(2);
		display->refresh();
		delay(500);

		if (SD.exists("/MAPPINGS.TXT")){
			if (SerialFlash.exists("MAPPINGS.TXT")){
				SerialFlash.remove("MAPPINGS.TXT");
				if (SerialFlash.exists("MAPPINGS.TXT")){
					display->write_text(1, 0, "Delete failed; try  ", 20);
					display->write_text(2, 0, "calling Load Samples", 20);
					display->refresh();
					Mapping::loadMappings();
					delay(1000);
					return Menu::mainMenu;
				}
			}
				
			File f = SD.open("/MAPPINGS.TXT");
			const char *filename = f.name();
			uint32_t length = f.size();
			snprintf(buf, sizeof(buf), "Copying %s           ", filename);
			display->write_text(1, 0, buf, 20);
			display->refresh();
			
			if (SerialFlash.createErasable(filename, length)) {
				SerialFlashFile ff = SerialFlash.open(filename);
				if (ff) {
					// copy data loop
					uint32_t count = 0;
					while (count < length) {
						uint8_t buf[256];
						uint16_t n;
						n = f.read(buf, 256);
						ff.write(buf, n);
						count = count + n;
					}
					ff.close();
					delay(500);
				} 
				else {
					display->write_text(1, 0, "Flash error open    ", 20);
					display->refresh();
					Mapping::loadMappings();
					delay(2000);
					return Menu::mainMenu;
				}
			}
			else {
				display->write_text(1, 0, "Flash error create; ", 20);
				display->write_text(2, 0, "try Load Samples    ", 20);
				display->refresh();
				Mapping::loadMappings();
				delay(2000);
				return Menu::mainMenu;
			}
			f.close();
		}
		else {
			display->write_text(1, 0, "File not found;     ", 20);
			display->write_text(2, 0, "ensure MAPPINGS.TXT ", 20);
			display->write_text(3, 0, "is on SD card.      ", 20);
			display->refresh();
			Mapping::loadMappings();
			delay(2000);
			return Menu::mainMenu;
		}

		display->write_text(1, 0, "Load from SD done.  ", 20);
		display->refresh();
		Mapping::loadMappings();
		setMenuPosition(0);
		EEPROM.update(EEPROM_KIT_INDEX, 0);
		Pad::loadAllSamples(0);
		delay(1000);
		return Menu::mainMenu;
	}
	else if (button.longPressEvent() || (button.releaseEvent() && getMenuPosition(0) == 0)){
		return Menu::mainMenu;
	}
		
	return NULL;
}
