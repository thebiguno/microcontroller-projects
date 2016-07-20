#include "Mapping.h"

#include "../menu/Menu.h"
#include "../Pad.h"

using namespace digitalcave;

Mapping Mapping::mappings[KIT_COUNT];
uint8_t Mapping::kitCount;
uint8_t Mapping::selectedKit;

void Mapping::loadMappings(){
	uint8_t state = STATE_NEWLINE;

	int8_t kitIndex = -1;
	
	//Init the mapping objects
	for(uint8_t i = 0; i < KIT_COUNT; i++){
		//Zero out the kit names
		for(uint8_t j = 0; j < KITNAME_STRING_SIZE; j++){
			mappings[i].kitName[j] = 0x00;
		}
		//Zero out the filename details for each pad
		for (uint8_t j = 0; j < PAD_COUNT; j++){
			for (uint8_t k = 0; k < FILENAME_COUNT; k++){
				for (uint8_t l = 0; l < FILENAME_PREFIX_STRING_SIZE; l++){
					mappings[i].filenamePrefixes[j][k][l] = 0x00;
				}
			}
			
			mappings[i].filenamePrefixCount[j] = 0;
		}
	}

	//These are the indices into the kit name string and the mapping lines
	uint8_t kitNameIndex = 0;
	uint8_t mappingIndex = 0;
	
	//Holds the first character in the mapping key.  0xFF implies not set.
	char lastMappingKey = 0xFF;

	//This is the current pad index.  Looked up based on the mapping key.  0xFF implies not set.
	uint8_t padIndex = 0xFF;
	//Current filename index.  Starts at 0, and increments with each comma separated filename.
	// Must be less than FILENAME_COUNT.
	uint8_t filenameIndex = 0;

	SerialFlashFile mappingsFile = SerialFlash.open("MAPPINGS.TXT");
	if (!mappingsFile) {
		kitCount = 0;
		
		Menu::display->clear();
		Menu::display->write_text(0, 0, "No Mappings Found...", 20);
		Menu::display->write_text(1, 0, "Please copy samples ", 20);
		Menu::display->write_text(2, 0, "to the flash chip   ", 20);
		Menu::display->write_text(3, 0, "via Serial or SD.   ", 20);
		Menu::display->refresh();
		delay(2000);
		Menu::display->clear();

		return;
	}
	
	char buffer[BUFFER_SIZE];		//Temporary buffer for reading data
	uint8_t count = BUFFER_SIZE;	//Result of file.read(); if this is less than requested, then the file has ended.
	while (count == BUFFER_SIZE){
		count = mappingsFile.read(buffer, BUFFER_SIZE);
	
		for (uint8_t i = 0; i < count; i++){
			if (state == STATE_INVALID){
				//When we are in an invalid state, the only way out is to read a newline.
				if (buffer[i] == '\n' || buffer[i] == '\r') {
					state = STATE_NEWLINE;
				}
			}
			else if (state == STATE_NEWLINE){
				//Alphanumeric starts a kitname
				if ((buffer[i] >= 'A' && buffer[i] <= 'Z') || (buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] >= '0' && buffer[i] <= '9')){
					kitIndex++;
					if (kitIndex >= KIT_COUNT){
						mappingsFile.close();
						kitCount = kitIndex + 1;
						return;
					}
					kitNameIndex = 0;
					for (uint8_t j = 0; j < KITNAME_STRING_SIZE; j++){
						mappings[kitIndex].kitName[j] = 0x00;	//Null out string
					}
					mappings[kitIndex].kitName[kitNameIndex++] = buffer[i];
					state = STATE_KITNAME;
				}
				//Tab starts a mapping
				else if (buffer[i] == '\t'){
					state = STATE_MAPPING;
					mappingIndex = 0;
					lastMappingKey = 0xFF;
					padIndex = 0xFF;
				}
				//Hash starts a comment
				else if (buffer[i] == '#'){
					state = STATE_COMMENT;
				}
				//Multiple new lines are valid
				else if (buffer[i] == '\n' || buffer[i] == '\r'){
					state = STATE_NEWLINE;
				}
				//Anything else is invalid
				else {
					state = STATE_INVALID;
				}
			}
			else if (state == STATE_COMMENT){
				//Once in a comment, we remain there until new line
				if (buffer[i] == '\n' || buffer[i] == '\r'){
					state = STATE_NEWLINE;
				}
			}
			else if (state == STATE_KITNAME){
				//Any valid ASCII character continues a kitname
				if (buffer[i] >= 0x20 && buffer[i] <= 0x7E){
					if (kitNameIndex < KITNAME_STRING_SIZE - 1){
						mappings[kitIndex].kitName[kitNameIndex++] = buffer[i];
					}
				}
				else if (buffer[i] == '\n' || buffer[i] == '\r'){
					state = STATE_NEWLINE;
				}
				else {
					state = STATE_INVALID;
				}
			}
			else if (state == STATE_MAPPING){
				//You need to have a kit name prior to sample mapping lines
				if (kitIndex == -1){
					state = STATE_INVALID;
				}
				//Newline
				else if (buffer[i] == '\n' || buffer[i] == '\r'){
					state = STATE_NEWLINE;
				}
				//Some character other than A-Z, 0-9, comma, period, colon, dash, underscore
				else if (!((buffer[i] >= 'A' && buffer[i] <= 'Z') || (buffer[i] >= '0' && buffer[i] <= '9') || buffer[i] == '.' || buffer[i] == ',' || buffer[i] == ':' || buffer[i] == '-' || buffer[i] == '_')){
					state = STATE_INVALID;
				}
				//First char of mapping key
				else if (mappingIndex == 0){
					lastMappingKey = buffer[i];
				}
				//Second char of mapping key
				else if (mappingIndex == 1){
					if (lastMappingKey == 'H' && buffer[i] == 'H') padIndex = 0;
					else if (lastMappingKey == 'S' && buffer[i] == 'N') padIndex = 1;
					else if (lastMappingKey == 'B' && buffer[i] == 'S') padIndex = 2;
					else if (lastMappingKey == 'T' && buffer[i] == '1') padIndex = 3;
					else if (lastMappingKey == 'C' && buffer[i] == 'R') padIndex = 4;
					else if (lastMappingKey == 'T' && buffer[i] == '2') padIndex = 5;
					else if (lastMappingKey == 'T' && buffer[i] == '3') padIndex = 6;
					else if (lastMappingKey == 'S' && buffer[i] == 'P') padIndex = 7;
					else if (lastMappingKey == 'R' && buffer[i] == 'D') padIndex = 8;
					else if (lastMappingKey == 'X' && buffer[i] == '0') padIndex = 9;
					else if (lastMappingKey == 'X' && buffer[i] == '1') padIndex = 10;
					else {
						state = STATE_INVALID;
						padIndex = 0xFF;
					}
				}
				//Separator character (colon)
				else if (mappingIndex == 2){
					if (buffer[i] == ':') {
						filenameIndex = 0;
					}
					else {
						state = STATE_INVALID;
					}
				}
				//Filling up filename
				else if ((buffer[i] >= 'A' && buffer[i] <= 'Z') || (buffer[i] >= '0' && buffer[i] <= '9') || buffer[i] == '.' || buffer[i] == '-' || buffer[i] == '_'){
					if ((mappingIndex - 3) < FILENAME_PREFIX_STRING_SIZE - 1){
 						mappings[kitIndex].filenamePrefixes[padIndex][filenameIndex][mappingIndex - 3] = buffer[i];
					}
				}
				//Comma separated list for filenames
				else if (buffer[i] == ','){
					filenameIndex++;
					if (filenameIndex >= FILENAME_COUNT){
						filenameIndex = FILENAME_COUNT - 1;
					}
					mappings[kitIndex].filenamePrefixCount[padIndex] = filenameIndex + 1;	//Keep track of how many filenames for each pad
				}
				//Something else
				else {
					state = STATE_INVALID;
				}
				
				mappingIndex++;
			}
		}
	}
	
	mappingsFile.close();
	kitCount = kitIndex + 1;
}

Mapping* Mapping::getSelectedMapping(){
	return &mappings[selectedKit];
}

uint8_t Mapping::getKitCount(){
	return kitCount;
}

uint8_t Mapping::getSelectedKit(){
	return selectedKit;
}
void Mapping::setSelectedKit(uint8_t kitIndex){
	if (kitIndex >= kitCount) kitIndex = kitCount - 1;
	selectedKit = kitIndex;
	
	Mapping* selected = &mappings[selectedKit];
	
	//Clear the sample volumes
	for (uint8_t i = 0; i < 18; i++){
		selected->sampleVolumes[i] = 0x00;
	}

	//Loop through each pad defined in the current mapping
	for (uint8_t i = 0; i < PAD_COUNT; i++){
		for (uint8_t j = 0; j < selected->filenamePrefixCount[i]; j++){
			//The filename prefix must be at least three chars
			if (strlen(selected->filenamePrefixes[i][j]) < 3) continue;
			
			SerialFlash.opendir();
			while (1) {
				char filename[16];
				unsigned long filesize;

				if (SerialFlash.readdir(filename, sizeof(filename), filesize)) {
					uint8_t filenamePrefixLength = strlen(selected->filenamePrefixes[i][j]);
					if (filenamePrefixLength > 6) filenamePrefixLength = 6;
					
					//Check that this filename starts with the currently assigned filename prefix
					if (strncmp(selected->filenamePrefixes[i][j], filename, filenamePrefixLength) != 0) {
						continue;
					}

					uint8_t pedalPosition = 0;

					//Check that there is a valid character immediately after the filename prefix.
					// Depending on the pad type, this may be an underscore, a 'B' (Bell), or 0-9 A-F (HiHat level).
					if (Pad::getPad(i)->getPadType() == PAD_TYPE_DRUM){
						if (filename[filenamePrefixLength] != '_'){
							continue;	//Invalid filename, missing '_' before volume.
						}
						pedalPosition = 0;
					}
					else if (Pad::getPad(i)->getPadType() == PAD_TYPE_CYMBAL){
						//Check that the filename pedal position is valid (_ or B).  The pedal position is the first character after the prefix.
						char filePedalPosition = filename[filenamePrefixLength];
						if (filePedalPosition == '_') pedalPosition = 0;
						else if (filePedalPosition == 'B') pedalPosition = 1;
						else {
							continue;	//Invalid volume
						}
					}
					else if (Pad::getPad(i)->getPadType() == PAD_TYPE_HIHAT){
						//Check that the filename pedal position is valid (0..F).  The pedal position is the first character after the prefix.
						char filePedalPosition = filename[filenamePrefixLength];
						if (filePedalPosition >= '0' && filePedalPosition <= '9') pedalPosition = filePedalPosition - 0x30;
						else if (filePedalPosition >= 'A' && filePedalPosition <= 'F') pedalPosition = filePedalPosition - 0x37;
						else if (filePedalPosition == 'K') pedalPosition = HIHAT_SPECIAL_CHIC;
						else if (filePedalPosition == 'P') pedalPosition = HIHAT_SPECIAL_SPLASH;
						else {
							continue;	//Invalid volume
						}
					}
					
					//Check that the filename volume is valid (0..F).  The volume is the second character after the prefix.
					char fileVolume = filename[filenamePrefixLength + 1];
					uint8_t volume;
					if (fileVolume >= '0' && fileVolume <= '9') volume = fileVolume - 0x30;
					else if (fileVolume >= 'A' && fileVolume <= 'F') volume = fileVolume - 0x37;
					else {
						continue;	//Invalid volume
					}

					selected->sampleVolumes[pedalPosition] |= _BV(volume);
				} 
				else {
					break; // no more files
				}
			}
		}
	}
}

char* Mapping::getKitName(){
	return kitName;
}

inline uint8_t getClosestVolume(int8_t closestVolume, uint8_t pedalPositionIndex, uint16_t* sampleVolumes){
	if (sampleVolumes[pedalPositionIndex] == 0) return 0xFF;
	
	//Start at the current bucket; if that is not a match, look up and down until a match is found.  At 
	// most this will take 16 tries (since there are 16 buckets)
	for(uint8_t i = 0; i < 16; i++){
		if (((closestVolume + i) <= 0x0F) && (sampleVolumes[pedalPositionIndex] & _BV(closestVolume + i))) {
			return closestVolume + i;
		}
		else if (((closestVolume - i) >= 0x00) && (sampleVolumes[pedalPositionIndex] & _BV(closestVolume - i))) {
			return closestVolume - i;
		}
	}
	
	return 0xFF;
}

uint8_t Mapping::getFilenames(uint8_t padIndex, double volume, uint8_t switchPosition, uint8_t pedalPosition, char** filenames){
	if (padIndex >= PAD_COUNT){
		return 0;
	}

	
	//Limit volume from 0 to 1
	if (volume < 0) volume = 0;
	else if (volume >= 1.0) volume = 1.0;

	//We find the closest match in fileCountByVolume
	int8_t closestVolume = volume * 16;		//Multiply by 16 to get into the 16 buckets of the samples
	
	for (uint8_t i = 0; i < filenamePrefixCount[padIndex]; i++){
		if (Pad::getPad(padIndex)->getPadType() == PAD_TYPE_DRUM){
			closestVolume = getClosestVolume(closestVolume, 0, sampleVolumes);

			snprintf(filenames[i], FILENAME_STRING_SIZE, "%s_%X.RAW", filenamePrefixes[padIndex][i], closestVolume);
		}
		else if (Pad::getPad(padIndex)->getPadType() == PAD_TYPE_CYMBAL){
			//Find the closes match in pedal position.
			int8_t closestPedalPosition = 0;	//This is either 0 (normal ride) or 1 (bell)

			//Pedal position is more important than velocity; thus, we look for the closest match on
			// position first, and then once we find any sample closest to the selected pedal position,
			// we then look to find the closest velocity sample within that position.
			if (pedalPosition > 8 && sampleVolumes[1]){
				closestPedalPosition = 1;
				closestVolume = getClosestVolume(closestVolume, 1, sampleVolumes);
			}
			else {
				closestVolume = getClosestVolume(closestVolume, 0, sampleVolumes);
			}
			
			snprintf(filenames[i], FILENAME_STRING_SIZE, "%s%c%X.RAW", filenamePrefixes[padIndex][i], closestPedalPosition == 0 ? '_' : 'B', closestVolume);
		}
		else if (Pad::getPad(padIndex)->getPadType() == PAD_TYPE_HIHAT){
			if (pedalPosition == HIHAT_SPECIAL_CHIC || pedalPosition == HIHAT_SPECIAL_SPLASH){
				closestVolume = getClosestVolume(closestVolume, pedalPosition, sampleVolumes);
				if (closestVolume == 0xFF){
					return i;
				}
				else {
					snprintf(filenames[i], FILENAME_STRING_SIZE, "%s%c%X.RAW", filenamePrefixes[padIndex][i], pedalPosition == HIHAT_SPECIAL_CHIC ? 'K' : 'P', closestVolume);
				}
			}
			else {
				//Find the closes match in pedal position.
				int8_t closestPedalPosition = pedalPosition;	//Pedal position is already a 4 bit number

				//Pedal position is more important than velocity; thus, we look for the closest match on
				// position first, and then once we find any sample closest to the selected pedal position,
				// we then look to find the closest velocity sample within that position.
				for(uint8_t i = 0; i < 16; i++){
					//First we check if there is anything in the sampleVolumes array at this index; if so, we go with it.
					if (((closestPedalPosition + i) <= 0x0F) && (sampleVolumes[closestPedalPosition + i])) {
						//Remember what pedal position we are looking at
						closestPedalPosition = closestPedalPosition + i;
						closestVolume = getClosestVolume(closestVolume, closestPedalPosition, sampleVolumes);
						break;
					}
					//Likewise on the low side.
					if (((closestPedalPosition - i) <= 0x0F) && (sampleVolumes[closestPedalPosition - i])) {
						//Remember what pedal position we are looking at
						closestPedalPosition = closestPedalPosition - i;
						closestVolume = getClosestVolume(closestVolume, closestPedalPosition, sampleVolumes);
						break;
					}
				}

				snprintf(filenames[i], FILENAME_STRING_SIZE, "%s%X%X.RAW", filenamePrefixes[padIndex][i], closestPedalPosition, closestVolume);
			}
		}
		
	}

	
	
	
	if (padIndex < PAD_COUNT){
		filenames = (char**) this->filenamePrefixes[padIndex];
		for (int8_t i = FILENAME_COUNT - 1; i >= 0; i--){
			if (filenamePrefixes[padIndex][i][0] != 0x00) return i;
		}
	}
	return 0;
}