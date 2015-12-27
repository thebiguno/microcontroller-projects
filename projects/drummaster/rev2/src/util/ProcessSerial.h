#ifndef PROCESS_SERIAL_H
#define PROCESS_SERIAL_H

#include <stdint.h>

#include <SD.h>

#include "../hardware.h"

#define STATE_START			0
#define STATE_CONTENT		1

#define SD_FILE_BUFFER_SIZE	256

//Special bytes
#define BYTE_START			0x7e
#define BYTE_ESCAPE			0x7d
#define BYTE_FILENAME_END	0x7c

namespace digitalcave {

	class ProcessSerial {
		private:
			//A file handle for writing
			File file;
			
			uint8_t sdFileBuffer[SD_FILE_BUFFER_SIZE];
			uint16_t sdFileBufferCounter;
			char filename[FILENAME_STRING_SIZE];
			uint8_t filenameCounter;
			uint8_t state;
			
			//Set to 1 if the previous byte was an escape char.
			uint8_t escape;
			
			//Zeroes out the filename
			void clearFilename();
			
		public:
			ProcessSerial();
			
			/*
			 * Reads the serial port and copies files to the SD card as they are sent from the computer
			 */
			void poll();
	};
	
}

#endif
