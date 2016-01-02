#include "LoadSamplesFromSD.h"

using namespace digitalcave;

static const char* labels[] = {
	"Cancel             ",
	"Start (No Format)  ",
	"Start (With Format)",
};

LoadSamplesFromSD::LoadSamplesFromSD() : Menu(3){
}

Menu* LoadSamplesFromSD::handleAction(){
	display->write_text(0, 0, "Load From SD         ", 20);
	
	int8_t positionOffset = getPositionOffset();
	writeSelection(positionOffset);

	display->write_text(1, 1, labels[getMenuPosition(positionOffset - 1)], 19);
	display->write_text(2, 1, labels[getMenuPosition(positionOffset)], 19);
	display->write_text(3, 1, labels[getMenuPosition(positionOffset + 1)], 19);

	if (button.longPressEvent() || (button.releaseEvent() && getMenuPosition(0) == 0)){
		display->clear();
		return Menu::settings;
	}
	else if (button.releaseEvent() && (getMenuPosition(0) == 1 || getMenuPosition(0) == 2)){
		display->clearRow(2);
		display->clearRow(3);
		
		//Whether format option was selected
		uint8_t format = (getMenuPosition(0) == 2);
		
		if (format){
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
			uint16_t time = 0;
			while (!SerialFlash.ready()) {
				if (millis() - last_millis > 1000) {
					time++;
					last_millis = millis();
					snprintf(buf, sizeof(buf), "Formatting %dMB%c%c%c    ", (uint16_t) (size >> 20), (i > 0 ? '.' : ' '), (i > 1 ? '.' : ' '), (i > 2 ? '.' : ' '));
					display->write_text(1, 0, buf, 20);
					snprintf(buf, sizeof(buf), "(%d sec. elapsed)        ", time);
					display->write_text(2, 0, buf, 20);
					display->refresh();
					i = (i + 1) & 0x03;
				}
			}
		}
		
		File folder = SD.open("/");
		while (1) {
			// open a file from the SD card
			File f = folder.openNextFile();
			if (!f) break;
			const char *filename = f.name();
			if (strlen(filename) > 4){
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
						display->write_text(1, 0, "Error Flash File Open", 20);
						display->refresh();
						setMenuPosition(0);
						Mapping::loadMappings();
						delay(2000);
						return Menu::settings;
					}
				}
				else {
					display->write_text(1, 0, "Error Flash Create   ", 20);
					display->write_text(2, 0, "There may be no room ", 20);
					display->write_text(3, 0, "left; try formatting ", 20);
					display->refresh();
					setMenuPosition(0);
					Mapping::loadMappings();
					delay(2000);
					return Menu::settings;
				}
			}
			f.close();
		}
		folder.close();
		display->write_text(1, 0, "Load from SD done.  ", 20);
		setMenuPosition(0);
		display->refresh();
		Mapping::loadMappings();
		KitSelect::loadKitIndexFromEeprom();
		delay(1000);
		display->clear();
		
		return Menu::mainMenu;
	}
	return NULL;
}
