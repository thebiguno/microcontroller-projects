#include "LoadSamplesFromSD.h"

using namespace digitalcave;

LoadSamplesFromSD::LoadSamplesFromSD(){
}

Menu* LoadSamplesFromSD::handleAction(){
	int8_t start = encoder.read() / 2;

	if (start < 0) {
		start = 1;
		encoder.write(2);
	}
	else if (start > 1){
		start = 0;
		encoder.write(0);
	}
	
	if (start) display->write_text(1, 0, "<Start>             ", 20);
	else        display->write_text(1, 0, "<Cancel>            ", 20);
	
	if (button.fallingEdge()){
		if (start){
			uint8_t id[3];
			SerialFlash.readID(id);
			uint32_t size = SerialFlash.capacity(id);
	
			snprintf(buf, sizeof(buf), "Erasing %dMB          ", (uint16_t) (size >> 20));
			display->write_text(1, 0, buf, 20);
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
				}
				f.close();
			}
			folder.close();
			display->write_text(1, 0, "Load From SD Done   ", 20);
			encoder.write(0);
			display->refresh();
			EEPROM.update(EEPROM_KIT_INDEX, 0);
			Pad::loadAllSamples(0, ((KitSelect*) Menu::kitSelect)->getMapping());		//Reload the sample mapping
			delay(1000);
			display->clear();
		
		}
		else {
			display->write_text(1, 0, "                    ", 20);
		}
		
		return Menu::mainMenu;
	}
	return NULL;
}
