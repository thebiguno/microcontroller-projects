#include "LoadSamplesFromSerial.h"

//Buffer sizes
#define USB_BUFFER_SIZE	128
#define SD_BUFFER_SIZE	16384

//State machine
#define STATE_START			0
#define STATE_CONTENT		1

//Special bytes
#define BYTE_START			0x7e
#define BYTE_ESCAPE			0x7d
#define BYTE_FILENAME_END	0x7c


using namespace digitalcave;

LoadSamplesFromSerial::LoadSamplesFromSerial() : Menu(2){
}

Menu* LoadSamplesFromSerial::handleAction(){
	display->write_text(0, 0, "Load From Serial     ", 20);
	
	if (getMenuPosition(0) == 0){
		display->write_text(1, 0, (char) 0x7E);
		display->write_text(2, 0, ' ');
	}
	else {
		display->write_text(1, 0, ' ');
		display->write_text(2, 0, (char) 0x7E);
	}
	
	display->write_text(1, 1, "Start               ", 19);
	display->write_text(2, 1, "Cancel              ", 19);

	if (button.releaseEvent() && getMenuPosition(0) == 0){
		//We start by formatting the flash...
		uint8_t id[3];
		SerialFlash.readID(id);
		uint32_t size = SerialFlash.capacity(id);
		SerialFlash.eraseAll();
		
		//Show progress while erasing...
		uint32_t last_millis = millis();
		uint8_t i = 0;
		while (!SerialFlash.ready()) {
			if (millis() - last_millis > 500) {
				last_millis = millis();
				display->write_text(1, 0, "Formatting Flash...  ", 20);
				snprintf(buf, sizeof(buf), "Erasing %dMB%c%c%c       ", (uint16_t) (size >> 20), (i > 0 ? '.' : ' '), (i > 1 ? '.' : ' '), (i > 2 ? '.' : ' '));
				display->write_text(2, 0, buf, 20);
				display->refresh();
				i = (i + 1) & 0x03;
			}
		}
		
		delay(1000);

		//Once the flash formatting has started, we delete all existing files on SD card...
		display->write_text(1, 0, "Deleting SD Files... ", 20);
		display->refresh();
		
		char filename[FILENAME_STRING_SIZE + 1];
		
		File sdRoot = SD.open("/");
		File sdFile;
		while (sdFile = sdRoot.openNextFile()){
			if (!sdFile.isDirectory()){
				filename[0] = '/';
				strncpy(filename + 1, sdFile.name(), FILENAME_STRING_SIZE);
				snprintf(buf, sizeof(buf), "%s                      ", filename);
				display->write_text(2, 0, buf, 20);
				display->refresh();
				sdFile.close();
				SD.remove(filename);
			}
		}
		sdRoot.close();
		display->clearRow(2);
		
		//Next we start listening for the upload program
		while(!Serial.available()){
			display->write_text(1, 0, "Start upload program ", 20);
			display->refresh();
		}
		
		uint8_t state = STATE_START;
		uint8_t escape = 0;
		
		uint8_t usbBuffer[USB_BUFFER_SIZE];
		uint8_t sdBuffer[SD_BUFFER_SIZE];
		
		uint16_t sdBufferIndex = 0;
		uint8_t filenameIndex = 0;
		
		uint32_t lastReceiveTime = millis();
		
		//We assume the serial receive part is finished when we have not received something for 2 seconds
		while(Serial.available() || lastReceiveTime + 2000 > millis()){
			uint16_t available = Serial.readBytes((char*) usbBuffer, USB_BUFFER_SIZE);
			if (available){
				display->write_text(1, 0, "Receiving...         ", 20);
				lastReceiveTime = millis();
			}
			else {
				display->write_text(1, 0, "Waiting...           ", 20);
				display->write_text(2, 0, "                     ", 20);
			}
			display->refresh();

			for (uint16_t usbBufferIndex = 0; usbBufferIndex < available; usbBufferIndex++){
				uint8_t b = usbBuffer[usbBufferIndex];
				
				if (state == STATE_START){
					//Start byte.  Repeat start is fine.
					if (b == BYTE_START){
						for (uint8_t i = 0; i < FILENAME_STRING_SIZE + 1; i++){
							filename[i] = 0x00;
						}
						filenameIndex = 0;
					}
					//Valid characters are A-Z, 0-9, comma, period, colon, dash, underscore
					else if ((b >= 'A' && b <= 'Z') || (b >= '0' && b <= '9') || b == '.' || b == ',' || b == ':' || b == '-' || b == '_'){
						filename[filenameIndex++] = b;
						if (filenameIndex >= FILENAME_STRING_SIZE){
							display->write_text(1, 0, "Error                ", 20);
							display->write_text(2, 0, "Filename too long    ", 20);
							display->refresh();
							delay(2000);
							return Menu::mainMenu;
						}
					}
					//Filename end character
					else if (b == BYTE_FILENAME_END){
						if (filenameIndex == 0){
							display->write_text(1, 0, "Error                ", 20);
							display->write_text(2, 0, "Empty filename       ", 20);
							display->refresh();
							delay(2000);
							return Menu::mainMenu;
						}
						
						snprintf(buf, sizeof(buf), "%s                      ", filename);
						display->write_text(2, 0, buf, 20);
						display->refresh();

						//Open the file for writing
						sdFile = SD.open(filename, FILE_WRITE);
						
						//Change state
						state = STATE_CONTENT;
						sdBufferIndex = 0;
						escape = 0;
					}
					//Invalid character
					else {
						display->write_text(1, 0, "Error                ", 20);
						display->write_text(2, 0, "Bad char in filename ", 20);
						display->refresh();
						delay(2000);
						return Menu::mainMenu;
					}
				}
				else if (state == STATE_CONTENT){
					//Previous byte was escaped; unescape and add to buffer
					if (escape){
						escape = 0;
						sdBuffer[sdBufferIndex++] = b ^ 0x20;
					}
					//Escape the next byte
					else if (b == BYTE_ESCAPE){
		// 				Serial.println("esc");
						escape = 1;
					}
					//End of file
					else if (b == BYTE_START){
		// 				Serial.println("End of file");
						state = STATE_START;
						sdFile.write(sdBuffer, sdBufferIndex);
						sdFile.close();
						sdBufferIndex = 0;
					}
					//Normal byte; add to buffer
					else {
						sdBuffer[sdBufferIndex++] = b;
					}
					
					//The buffer is filled; write to SD card
					if (sdBufferIndex >= SD_BUFFER_SIZE){
						sdFile.write(sdBuffer, SD_BUFFER_SIZE);
						sdBufferIndex = 0;
					}
				}
			}
		}
		
		display->refresh();
		delay(1000);
		
		//Once we have received the files, we check if the flash is done formatting...
		while (!SerialFlash.ready()) {
			delay(500);
			display->write_text(1, 0, "Waiting for Flash    ", 20);
			display->write_text(2, 0, "Format to finish...  ", 20);
			display->refresh();
		}
		
		//We always need to copy a MAPPINGS file... the client checks for it, but good to check here too
		if (SD.exists("/MAPPINGS.TXT")){
			File f = SD.open("/MAPPINGS.TXT");
			const char *filename = f.name();
			uint32_t length = f.size();
			snprintf(buf, sizeof(buf), "Copying %s           ", filename);
			display->write_text(1, 0, buf, 20);
			display->refresh();
			
			if (SerialFlash.createErasable(filename, length)) {
				SerialFlashFile flashFile = SerialFlash.open(filename);
				if (flashFile) {
					// copy data loop
					uint32_t count = 0;
					while (count < length) {
						uint8_t buf[256];
						uint16_t n;
						n = f.read(buf, 256);
						flashFile.write(buf, n);
						count = count + n;
					}
					flashFile.close();
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
		
		//Copy all the .RAW files
		sdRoot = SD.open("/");
		while (sdFile = sdRoot.openNextFile()) {
			const char *filename = sdFile.name();
			if(strlen(filename) > 4 && !strcmp(filename + strlen(filename) - 4, ".RAW")){
				uint32_t length = sdFile.size();
				snprintf(buf, sizeof(buf), "Copying %s           ", filename);
				display->write_text(1, 0, buf, 20);
				display->refresh();
		
				// Create the file on the Flash chip and copy data
				if (SerialFlash.create(filename, length)) {
					SerialFlashFile flashFile = SerialFlash.open(filename);
					if (flashFile) {
						// copy data loop
						uint32_t count = 0;
						while (count < length) {
							uint8_t buf[256];
							uint16_t n;
							n = sdFile.read(buf, 256);
							flashFile.write(buf, n);
							count = count + n;
						}
						flashFile.close();
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
			sdFile.close();
		}
		sdRoot.close();

		display->write_text(1, 0, "Load from Serial    ", 20);
		display->write_text(2, 0, "Finished            ", 20);
		setMenuPosition(0);
		display->refresh();
		EEPROM.update(EEPROM_KIT_INDEX, 0);
		Mapping::loadMappings();
		Pad::loadAllSamples(0);
		delay(1000);

		return Menu::mainMenu;
	}
	else if (button.longPressEvent() || (button.releaseEvent() && getMenuPosition(0) == 1)){
		return Menu::mainMenu;
	}
		
	return NULL;
}