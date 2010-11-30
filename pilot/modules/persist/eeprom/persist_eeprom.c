/*
 * LYPR530 implementation 
 */
#include "../persist.h"
#include "../../../../../lib/eeprom/eeprom.h"

void persist_init(){
    ;
}

//TODO Change section safety from 0x7 to a AVR-specific value based on MMCU

uint8_t persist_write(uint8_t section, uint8_t address, uint8_t *data, uint8_t length){
	//There are 512 bytes in ATMega168 EEPROM; we use three bits for the section 
	// address and the remaining 6 for address.  This gives at most 0x3F bytes
	// for each section; we also verify the length to make sure there is not an 
	// overflow.
	if ((address + length) >= 0x3F) length = (0x3F - address);
	
	uint16_t eeprom_address = ((section & 0x7) << 6) | (address & 0x3F);
	for (uint8_t i = 0; i < length; i++){
		eeprom_write_c(eeprom_address++, data[i]);
	}
	
	return length;
}

uint8_t persist_read(uint8_t section, uint8_t address, uint8_t *data, uint8_t length){
	if ((address + length) >= 0x3F) length = (0x3F - address);
	
	uint16_t eeprom_address = ((section & 0x7) << 6) | (address & 0x3F);
	for (uint8_t i = 0; i < length; i++){
		data[i] = eeprom_read_c(eeprom_address++);
	}
	
	return length;	
}
