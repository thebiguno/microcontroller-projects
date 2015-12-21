#include "Mapping.h"

using namespace digitalcave;

Mapping Mapping::mappings[KIT_COUNT];
uint8_t Mapping::kitCount;

void Mapping::loadMappings(){
	uint8_t state = STATE_NEWLINE;

	int8_t kitIndex = -1;
	
	//Init the mapping objects
	for(uint8_t i = 0; i < KIT_COUNT; i++){
		mappings[i].kitIndex = 0xFF;
		for(uint8_t j = 0; j < KITNAME_STRING_SIZE; j++){
			mappings[i].kitName[j] = 0x00;
		}
		for (uint8_t j = 0; j < PAD_COUNT; j++){
			for(uint8_t k = 0; k < FILENAME_STRING_SIZE; k++){
				mappings[i].filenamePrefixes[j][k] = 0x00;
			}
		}
	}

	//These are the indices into the kit name string and the mapping lines
	uint8_t kitNameIndex = 0;
	uint8_t mappingIndex = 0;
	
	//Holds the first character in the mapping key.  0xFF implies not set.
	char lastMappingKey = 0xFF;

	//This is the current pad index.  Looked up based on the mapping key.  0xFF implies not set.
	uint8_t padIndex = 0xFF;

	SerialFlashFile mappingsFile = SerialFlash.open("MAPPINGS.TXT");
	if (!mappingsFile) {
		kitCount = 0;
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
					mappings[kitIndex].kitIndex = kitIndex;
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
					Serial.print("Kit name: ");
					Serial.println(mappings[kitIndex].kitName);
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
						for (uint8_t j = 0; j < FILENAME_STRING_SIZE; j++){
							mappings[kitIndex].filenamePrefixes[padIndex][j] = 0x00;	//Null out string
						}
					}
					else {
						state = STATE_INVALID;
					}
				}
				//Filling up filename
				else if ((buffer[i] >= 'A' && buffer[i] <= 'Z') || (buffer[i] >= '0' && buffer[i] <= '9') || buffer[i] == '.' || buffer[i] == ',' || buffer[i] == '-' || buffer[i] == '_'){
					if ((mappingIndex - 3) < FILENAME_STRING_SIZE - 1){
 						mappings[kitIndex].filenamePrefixes[padIndex][mappingIndex - 3] = buffer[i];
					}
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

Mapping* Mapping::getMapping(uint8_t index){
	if (index < kitCount) return &mappings[index];
	else if (kitCount > 0) return &mappings[kitCount - 1];
	else return &mappings[0];
}

uint8_t Mapping::getKitCount(){
	return kitCount;
}

uint8_t Mapping::getKitIndex(){
	return kitIndex;
}

char* Mapping::getKitName(){
	return kitName;
}

char* Mapping::getFilenamePrefix(uint8_t padIndex){
	if (padIndex < PAD_COUNT) return filenamePrefixes[padIndex];
	else return NULL;
}