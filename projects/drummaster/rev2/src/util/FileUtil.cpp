#include "FileUtil.h"

using namespace digitalcave;

copyFolderToFlash(const char* folderName, CharDisplay display){
	char buf[21];
	if (!SD.begin(CS_SD)) {
		char_display.clear();
		char_display.write_text(0, 0, "SD Error", 8);
		char_display.refresh();
		delay(1000);
		return;
	}
	SerialFlash.begin(CS_FLASH);
	uint8_t id[3];
	SerialFlash.readID(id);
	uint32_t size = SerialFlash.capacity(id);
	
	char_display.clear();
	snprintf(buf, sizeof(buf), "Erasing %dMB...       ", (uint16_t) (size >> 20));
	char_display.write_text(0, 0, buf, sizeof(buf));
	char_display.refresh();
	
	SerialFlash.eraseAll();
	
	//Show progress while erasing...
	uint32_t last_millis = millis();
	uint8_t row = 1;
	uint8_t col = 0;
	while (!SerialFlash.ready()) {
		if (millis() - last_millis > 2000) {
			last_millis = millis();
			char_display.write_text(row, col, '.');
			col++;
			if (col >= DISPLAY_COLS){
				col = 0;
				row++;
			}
			char_display.refresh();
		}
	}

	char_display.clear();
	snprintf(buf, sizeof(buf), "Copy SD...                 ");
	char_display.write_text(0, 0, buf, sizeof(buf));
	char_display.refresh();
	
	File folder = SD.open(folderName);
	while (1) {
		// open a file from the SD card
		File f = folder.openNextFile();
		if (!f) break;
		const char *filename = f.name();
		uint32_t length = f.size();
		snprintf(buf, sizeof(buf), "%s                       ", filename);
		char_display.write_text(1, 0, buf, sizeof(buf));
		char_display.refresh();
		
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
				char_display.clear();
				char_display.write_text(0, 0, "Flash Error Open", 17);
				char_display.refresh();
				delay(1000);
				return;
			}
		}
		else {
			char_display.clear();
			char_display.write_text(0, 0, "Flash Error Create", 18);
			char_display.refresh();
			delay(1000);
			return;
		}
		f.close();
	}
	folder.close();
	char_display.clear();
}