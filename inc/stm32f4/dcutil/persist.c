#include <dcutil/persist.h>

void persist_write(uint32_t address, uint8_t* data, uint16_t length){
	//eeprom_update_block(data, (void*) (uint16_t) address, length);
}

void persist_read(uint32_t address, uint8_t* data, uint16_t length){
	//eeprom_read_block(data, (void*) (uint16_t) address, length);
}
