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
#define KITNAME_STRING_SIZE		20
#define FILENAME_STRING_SIZE	7
//Maximum number of kits.  Allocates enough memory to load all these kits, so keep the number low
#define KIT_COUNT				8

namespace digitalcave {

	class Mapping {
		private:
			static Mapping mappings[KIT_COUNT];
			static uint8_t kitCount;
			
			uint8_t kitIndex;
			char kitName[KITNAME_STRING_SIZE];
			char filenamePrefixes[PAD_COUNT][FILENAME_STRING_SIZE];
			
		public:
			//Loads the kit mappings from SPI flash.  Once loaded, you can then call 
			// getMappings() to get the filename prefixes needed to load into each pad.
			static void loadMappings();
			
			//Returns the specified mapping which was previously loaded with loadKits()
			static Mapping* getMapping(uint8_t index);
			
			//Returns the total number of kits defined in MAPPINGS.TXT
			static uint8_t getKitCount();
			
			uint8_t getKitIndex();
			
			char* getKitName();
			
			char* getFilenamePrefix(uint8_t padIndex);
	};
	
}

#endif