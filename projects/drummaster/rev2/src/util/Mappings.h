#ifndef MAPPINGS_H
#define MAPPINGS_H

#include <stdint.h>

#include <SerialFlash.h>

#include "../hardware.h"

#define STATE_INVALID		0
#define STATE_NEWLINE		1
#define STATE_COMMENT		2
#define STATE_KITNAME		3
#define STATE_MAPPING		4

//The size of the buffer used when reading in kit mappings
#define BUFFER_SIZE		256

//The size of the strings for kit name and filename prefix.  Includes null char at the end.
#define KITNAME_STRING_SIZE		21
#define FILENAME_STRING_SIZE	33

namespace digitalcave {

	class Mappings {
		private:
			static char kitName[KITNAME_STRING_SIZE];				//Used as return pointer for getKitName
			static char fileNames[PAD_COUNT][FILENAME_STRING_SIZE];	//First index is pad, second is filename

		public:
			//Loads the kit mappings from SPI flash, and returns the kit name at the given index.
			// Once loaded, you can then call getFilename() to get the filename prefixes needed
			// to load into each pad.
			static char* loadKit(uint8_t kitIndex);
			
			//Returns the filename prefix for the given pad.
			static char* getFilename(uint8_t padIndex);
	};
	
}

#endif