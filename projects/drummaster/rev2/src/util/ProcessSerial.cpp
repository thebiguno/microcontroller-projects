#include "ProcessSerial.h"

#define USB_BUFFER_SIZE				128
using namespace digitalcave;

ProcessSerial::ProcessSerial() : state(STATE_START){
	clearFilename();
}

void ProcessSerial::poll(){
	
	uint16_t available = Serial.available();
	if (!available) return;
	
	uint8_t usbBuffer[USB_BUFFER_SIZE] = {0};
	
	available = Serial.readBytes((char*) usbBuffer, USB_BUFFER_SIZE);

	for (uint16_t usbBufferIndex = 0; usbBufferIndex < available; usbBufferIndex++){
		uint8_t b = usbBuffer[usbBufferIndex];
		
		if (state == STATE_START){
			//Start byte.  Repeat start is fine.
			if (b == BYTE_START){
				clearFilename();
				filenameCounter = 0;
			}
			//Valid characters are A-Z, 0-9, comma, period, colon, dash, underscore
			else if ((b >= 'A' && b <= 'Z') || (b >= '0' && b <= '9') || b == '.' || b == ',' || b == ':' || b == '-' || b == '_'){
				filename[filenameCounter++] = b;
				if (filenameCounter >= FILENAME_STRING_SIZE){
					state = STATE_START;	//Filename is too long
					Serial.println("STATE_START (Filename too long)");
				}
			}
			//Filename end character
			else if (b == BYTE_FILENAME_END){
				if (filenameCounter == 0){
					state = STATE_START;
					Serial.println("STATE_START (Empty filename)");
				}
				//Delete any existing files of the same name
				//if (SD.exists(filename)){
				//	SD.remove(filename);
				//}
				//Open the file for writing
				//file = SD.open(filename, FILE_WRITE);
				
				//Change state
				state = STATE_CONTENT;
				sdFileBufferCounter = 0;
				escape = 0;
				Serial.println("STATE_CONTENT");
			}
			//Invalid character
			else {
				state = STATE_START;	
				Serial.print("STATE_START (Invalid byte in filename): '");
				Serial.print(b);
				Serial.println("'");
			}
		}
		else if (state == STATE_CONTENT){
			//Previous byte was escaped; unescape and add to buffer
			if (escape){
				escape = 0;
				sdFileBuffer[sdFileBufferCounter++] = b ^ 0x20;
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
				//file.write(sdFileBuffer, sdFileBufferCounter);
				//file.close();
			}
			//Normal byte; add to buffer
			else {
				sdFileBuffer[sdFileBufferCounter++] = b;
			}
			
			//The buffer is filled; write to SD card
			if (sdFileBufferCounter >= SD_FILE_BUFFER_SIZE){
				//file.write(sdFileBuffer, SD_FILE_BUFFER_SIZE);
				sdFileBufferCounter = 0;
			}
		}
	}
}

void ProcessSerial::clearFilename(){
	for (uint8_t i = 0; i < FILENAME_STRING_SIZE; i++){
		filename[i] = 0x00;
	}
}

