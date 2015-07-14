#include "FileUtil.h"

void copyFolderToFlash(const char* folderName, CharDisplay display){
	char buf[21];

	uint8_t id[3];
	SerialFlash.readID(id);
	uint32_t size = SerialFlash.capacity(id);
	
	snprintf(buf, sizeof(buf), "Erasing %dMB          ", (uint16_t) (size >> 20));
	display.write_text(2, 0, buf, 20);
	display.refresh();
	SerialFlash.eraseAll();
	
	//Show progress while erasing...
	uint32_t last_millis = millis();
	uint8_t i = 0;
	while (!SerialFlash.ready()) {
		if (millis() - last_millis > 500) {
			last_millis = millis();
			snprintf(buf, sizeof(buf), "Erasing %dMB%c%c%c       ", (uint16_t) (size >> 20), i > 0 ? '.' : ' ', i > 1 ? '.' : ' ', i > 2 ? '.' : ' ');
			display.write_text(2, 0, buf, 20);
			display.refresh();
			i = (i + 1) & 0x03;
		}
	}

	File folder = SD.open(folderName);
	while (1) {
		// open a file from the SD card
		File f = folder.openNextFile();
		if (!f) break;
		const char *filename = f.name();
		uint32_t length = f.size();
		snprintf(buf, sizeof(buf), "Copying %s           ", filename);
		display.write_text(2, 0, buf, 20);
		display.refresh();
		
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
				display.clear();
				display.write_text(2, 0, "Flash Error Open    ", 20);
				display.refresh();
				delay(1000);
				display.clear();
				return;
			}
		}
		else {
			display.clear();
			display.write_text(2, 0, "Flash Error Create  ", 20);
			display.refresh();
			delay(1000);
			display.clear();
			return;
		}
		f.close();
	}
	folder.close();
	display.write_text(2, 0, "Load Samples Done   ", 20);
	display.refresh();
	delay(1000);
	display.clear();
}

std::vector<String> getTopLevelFolders(){
	std::vector<String> result;
	result.push_back(String("<Cancel>"));
	
	File rootdir = SD.open("/");
	while (1){
		File f = rootdir.openNextFile();
		if (!f) break;
		if (f.isDirectory()) {
			result.push_back(String(f.name()));
		}
		f.close();
	}
	
	return result;
}