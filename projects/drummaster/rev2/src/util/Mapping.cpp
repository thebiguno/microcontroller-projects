#include "Mapping.h"

using namespace digitalcave;

uint8_t Mapping::loadKit(uint8_t index, Mapping* mapping){
// 	Serial.print("Loading kit index ");
// 	Serial.println(index);
	uint8_t state = STATE_NEWLINE;

	//We read the file until index == kitIndex, then load the kit file names at that location
	int8_t kitIndex = -1;
	
	//Init the mapping object
// 	Serial.println("Clearing mapping");
	mapping->kitIndex = 0xFF;
	for(uint8_t i = 0; i < KITNAME_STRING_SIZE; i++){
		mapping->kitName[i] = 0x00;
	}
	for (uint8_t i = 0; i < PAD_COUNT; i++){
		for(uint8_t j = 0; j < FILENAME_STRING_SIZE; j++){
			mapping->filenamePrefixes[i][j] = 0x00;
		}
	}
// 	Serial.println("Done clearing mapping");

	//These are the indices into the kit name string and the mapping lines
	uint8_t kitNameIndex = 0;
	uint8_t mappingIndex = 0;
	
	//Holds the first character in the mapping key.  0xFF implies not set.
	char lastMappingKey = 0xFF;

	//This is the current pad index.  Looked up based on the mapping key.  0xFF implies not set.
	uint8_t padIndex = 0xFF;

	SerialFlashFile mappingsFile = SerialFlash.open("MAPPINGS.TXT");
	if (!mappingsFile) {
		return 0;
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
					if (kitIndex == index){
						kitNameIndex = 0;
						mapping->kitIndex = kitIndex;
						for (uint8_t j = 0; j < KITNAME_STRING_SIZE; j++){
							mapping->kitName[j] = 0x00;	//Null out string
						}
						mapping->kitName[kitNameIndex++] = buffer[i];
						state = STATE_KITNAME;
					}
					else {
						state = STATE_COMMENT;		//Not really a comment, but we want to ignore it anyway
					}
				}
				//Tab starts a mapping, if we are on the right kit index
				else if (buffer[i] == '\t'){
					if (kitIndex == index){
						state = STATE_MAPPING;
						mapping->kitIndex = index;
						mappingIndex = 0;
						lastMappingKey = 0xFF;
						padIndex = 0xFF;
					}
					else {
						state = STATE_COMMENT;		//Not really a comment, but we want to ignore it anyway
					}
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
						mapping->kitName[kitNameIndex++] = buffer[i];
					}
				}
				else if (buffer[i] == '\n' || buffer[i] == '\r'){
// 					Serial.print("Kit name: ");
// 					Serial.println(mapping->kitName);
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
// 					Serial.print("Mapping ");
// 					Serial.print(padIndex);
// 					Serial.print(": ");
// 					Serial.println(mapping->filenamePrefixes[padIndex]);
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
							mapping->filenamePrefixes[padIndex][j] = 0x00;	//Null out string
						}
					}
					else {
						state = STATE_INVALID;
					}
				}
				//Filling up filename
				else if ((buffer[i] >= 'A' && buffer[i] <= 'Z') || (buffer[i] >= '0' && buffer[i] <= '9') || buffer[i] == '.' || buffer[i] == ',' || buffer[i] == '-' || buffer[i] == '_'){
					if ((mappingIndex - 3) < FILENAME_STRING_SIZE - 1){
						mapping->filenamePrefixes[padIndex][mappingIndex - 3] = buffer[i];
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
	
	kitIndex++;
	
	//If we have read at least to the requested kit name, return
// 	Serial.println();
// 	Serial.print("Returning kit name: ");
// 	Serial.println(mapping->kitName);
// 	Serial.println("Mappings:");
// 	for(uint8_t i = 0; i < PAD_COUNT; i++){
// 		Serial.println(mapping->filenamePrefixes[i]);
// 	}
// 	Serial.print("Total kits: ");
// 	Serial.println(kitIndex);
	return kitIndex;
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