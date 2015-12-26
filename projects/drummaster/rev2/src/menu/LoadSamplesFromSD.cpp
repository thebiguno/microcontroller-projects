#include "LoadSamplesFromSD.h"

using namespace digitalcave;

LoadSamplesFromSD::LoadSamplesFromSD() : Menu(2){
}

Menu* LoadSamplesFromSD::handleAction(){
	display->write_text(0, 0, "Load Samples         ", 20);
	
	display->write_text(getMenuPosition(1) + 1, 0, ' ');
	display->write_text(getMenuPosition(0) + 1, 0, (char) 0x7E);
	display->write_text(1, 1, "Cancel              ", 19);
	display->write_text(2, 1, "Start               ", 19);

	if (button.releaseEvent() && getMenuPosition(0) == 1){
		uint8_t id[3];
		SerialFlash.readID(id);
		uint32_t size = SerialFlash.capacity(id);

		snprintf(buf, sizeof(buf), "Erasing %dMB          ", (uint16_t) (size >> 20));
		display->write_text(1, 0, buf, 20);
		display->clearRow(2);
		display->refresh();
		SerialFlash.eraseAll();

		//Show progress while erasing...
		uint32_t last_millis = millis();
		uint8_t i = 0;
		while (!SerialFlash.ready()) {
			if (millis() - last_millis > 500) {
				last_millis = millis();
				snprintf(buf, sizeof(buf), "Erasing %dMB%c%c%c       ", (uint16_t) (size >> 20), (i > 0 ? '.' : ' '), (i > 1 ? '.' : ' '), (i > 2 ? '.' : ' '));
				display->write_text(1, 0, buf, 20);
				display->refresh();
				i = (i + 1) & 0x03;
			}
		}

		if (SD.exists("/MAPPINGS.TXT")){
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
					display->write_text(1, 0, "Flash error open    ", 20);
					display->write_text(1, 0, "MAPPINGS.txt        ", 20);
					display->refresh();
					delay(2000);
					display->clear();
					return Menu::mainMenu;
				}
			}
			else {
				display->clear();
				display->write_text(1, 0, "Flash error create; ", 20);
				display->write_text(2, 0, "is there room on the", 20);
				display->write_text(3, 0, "flash chip?         ", 20);
				display->refresh();
				delay(2000);
				display->clear();
				return Menu::mainMenu;
			}
			f.close();
		}
		else {
			display->write_text(1, 0, "File not found;     ", 20);
			display->write_text(2, 0, "ensure MAPPINGS.TXT ", 20);
			display->write_text(3, 0, "is on SD card.      ", 20);
			display->refresh();
			delay(2000);
			display->clear();
			return Menu::mainMenu;
		}
		
		File folder = SD.open("/");
		while (1) {
			// open a file from the SD card
			File f = folder.openNextFile();
			if (!f) break;
			const char *filename = f.name();
			if(strlen(filename) > 4 && !strcmp(filename + strlen(filename) - 4, ".RAW")){
				uint32_t length = f.size();
				snprintf(buf, sizeof(buf), "Copying %s           ", filename);
				display->write_text(1, 0, buf, 20);
				display->refresh();
		
				// Create the file on the Flash chip and copy data
				if (SerialFlash.create(filename, length)) {
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
						display->write_text(1, 0, "Flash error open    ", 20);
						snprintf(buf, sizeof(buf), "%s                  ", filename);
						display->write_text(2, 0, buf, 20);
						display->refresh();
						Mapping::loadMappings();
						delay(2000);
						display->clear();
						return Menu::mainMenu;
					}
				}
				else {
					display->clear();
					display->write_text(1, 0, "Flash error create; ", 20);
					display->write_text(2, 0, "is there room on the", 20);
					display->write_text(3, 0, "flash chip?         ", 20);
					display->refresh();
					Mapping::loadMappings();
					delay(2000);
					display->clear();
					return Menu::mainMenu;
				}
			}
			f.close();
		}
		folder.close();
		display->write_text(1, 0, "Load from SD done.  ", 20);
		setMenuPosition(0);
		display->refresh();
		EEPROM.update(EEPROM_KIT_INDEX, 0);
		Mapping::loadMappings();
		Pad::loadAllSamples(0);
		delay(1000);
		display->clear();
		
		return Menu::mainMenu;
	}
	else if (button.longPressEvent() || (button.releaseEvent() && getMenuPosition(0) == 0)){
		display->clear();
		return Menu::mainMenu;
	}
	return NULL;
}
