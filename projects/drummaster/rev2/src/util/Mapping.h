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

//The size of the strings for kit name.  Includes null char at the end.
#define KITNAME_STRING_SIZE				20

//Maximum number of kits.  Allocates enough memory to load all these kits, so keep the number low
#define KIT_COUNT						20

//Maximum number of filenames to be defined for a single pad.  More than one allows you to layer
// multiple samples to the same pad (i.e. hi hat and tambourine)
#define FILENAME_COUNT					2

namespace digitalcave {

	class Mapping {
		public:
			//Loads the kit mappings from SPI flash.  Once loaded, you can then call 
			// getMappings() to get the filename prefixes needed to load into each pad.
			static void loadMappings();
			
			//Returns the total number of kits defined in MAPPINGS.TXT
			static uint8_t getKitCount();
			
			//Get / set the selected kit index (or Mapping).  Setting this will analyze the mappings and 
			// populate state variables according to what samples are available.
			static uint8_t getSelectedKit();
			static Mapping* getSelectedMapping();
			static void setSelectedKit(uint8_t kitIndex);

			//Returns the kit name (20 char human readable label)
			char* getKitName();
			
			//Returns the number of filename prefixes that are available for this pad index,
			// and points to the filenames object.  The calling code can then start playing
			// each filename.
			uint8_t getFilenames(uint8_t padIndex, double volume, uint8_t switchPosition, uint8_t pedalPosition, char filenames[FILENAME_COUNT][FILENAME_STRING_SIZE]);

		private:
			static Mapping mappings[KIT_COUNT];		//All defined mappings, loaded from the mappings file
			static uint8_t kitCount;				//Total number of kits defined in the mappings file
			static uint8_t selectedKit;				//Currently selected kit
			
			char kitName[KITNAME_STRING_SIZE];
			
			/** Variables to store filename / pad mappings.  Initialized when loading mappings from file. **/
			uint8_t filenamePrefixCount[PAD_COUNT];
			char filenamePrefixes[PAD_COUNT][FILENAME_COUNT][FILENAME_PREFIX_STRING_SIZE];
			
			/** Variables to optimize returning a specific sample when playing sounds.  Initialized when
				setting the selected kit. **/
			//Multi dimensional bit mask showing which samples are available
			// at a given velocity (as bits along each 16 bit int) and at a given
			// pedal position or special effect (as entries in the array).
			//Drums only use index 0; cymbals with bells defined use index 0 and 1; ride cymbals
			// use all 16 + 2 special effects (chic and splash)
			uint16_t sampleVolumes[PAD_COUNT][FILENAME_COUNT][18];
	};
	
}

#endif