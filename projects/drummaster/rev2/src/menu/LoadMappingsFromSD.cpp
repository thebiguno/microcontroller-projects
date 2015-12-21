#include "LoadMappingsFromSD.h"

using namespace digitalcave;

LoadMappingsFromSD::LoadMappingsFromSD() : Menu(2){
}

Menu* LoadMappingsFromSD::handleAction(){
	display->write_text(0, 0, "Load Mappings        ", 20);
	
	display->write_text(getMenuPosition(1) + 1, 0, ' ');
	display->write_text(getMenuPosition() + 1, 0, (char) 0x7E);
	display->write_text(1, 1, "Start               ", 19);
	display->write_text(2, 1, "Cancel              ", 19);

	if (button.pressEvent() && getMenuPosition() == 0){
		display->write_text(1, 0, "Erasing Mappings.txt ", 20);
		display->clearRow(2);
		display->refresh();

		if (SD.exists("/MAPPINGS.TXT")){
			if (SerialFlash.exists("MAPPINGS.TXT")){
				SerialFlash.remove("MAPPINGS.TXT");
				if (SerialFlash.exists("MAPPINGS.TXT")){
					display->write_text(1, 0, "Delete Failed.      ", 20);
					display->write_text(2, 0, "Try formatting flash", 20);
					display->write_text(2, 0, "using Load Samples. ", 20);
					display->refresh();
					delay(1000);
					display->clear();
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
				} 
				else {
					display->clear();
					display->write_text(1, 0, "Flash Error Open    ", 20);
					display->refresh();
					delay(1000);
					display->clear();
					return Menu::mainMenu;
				}
			}
			else {
				display->clear();
				display->write_text(1, 0, "Flash Error Create  ", 20);
				display->refresh();
				delay(1000);
				display->clear();
				return Menu::mainMenu;
			}
			f.close();
		}
		else {
			display->write_text(1, 0, "File Not Found      ", 20);
			display->refresh();
			delay(1000);
			display->clear();
			return Menu::mainMenu;
		}

		display->write_text(1, 0, "Load From SD Done   ", 20);
		display->refresh();
		setMenuPosition(0);
		EEPROM.update(EEPROM_KIT_INDEX, 0);
		Pad::loadAllSamples(0);
		delay(1000);
		display->clear();
		return Menu::mainMenu;
	}
	else if (button.longPressEvent() || (button.pressEvent() && getMenuPosition() == 1)){
		display->clear();
		return Menu::mainMenu;
	}
		
	return NULL;
}
