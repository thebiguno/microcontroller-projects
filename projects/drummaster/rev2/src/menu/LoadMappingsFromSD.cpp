#include "LoadMappingsFromSD.h"

using namespace digitalcave;

LoadMappingsFromSD::LoadMappingsFromSD(){
}

Menu* LoadMappingsFromSD::handleAction(){
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
			display->write_text(1, 0, "Erasing Mappings.txt ", 20);
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
