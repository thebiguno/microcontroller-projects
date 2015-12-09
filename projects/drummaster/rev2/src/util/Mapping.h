#ifndef MAPPING_H
#define MAPPING_H

#include <stdint.h>

#include <SerialFlash.h>

#include "../hardware.h"

#define STATE_INVALID		0
#define STATE_NEWLINE		1
#define STATE_COMMENT		2
#define STATE_KITNAME		3
#define STATE_MAPPING		4

//The size of the buffer used when reading in kit mappings
#define BUFFER_SIZE		128

//The size of the strings for kit name and filename prefix.  Includes null char at the end.
#define KITNAME_STRING_SIZE		21
#define FILENAME_STRING_SIZE	7

namespace digitalcave {

	class Mapping {
		private:
			uint8_t kitIndex;
			char kitName[KITNAME_STRING_SIZE];
			char filenamePrefixes[PAD_COUNT][FILENAME_STRING_SIZE];
			

		public:
			//Loads the kit mappings from SPI flash, and returns the total count of all kits defined.
			// Once loaded, you can then call getFilename() to get the filename prefixes needed
			// to load into each pad.
			static uint8_t loadKit(uint8_t kitIndex, Mapping* mapping);
			
			uint8_t getKitIndex();
			
			char* getKitName();
			
			char* getFilenamePrefix(uint8_t padIndex);
	};
	
}

#endif