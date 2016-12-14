#include <dcutil/persist.h>
#include <avr/eeprom.h>

uint8_t persist_write(uint32_t address, uint8_t* data, uint16_t length){
	eeprom_update_block(data, (void*) (uint16_t) address, length);
	return 0;
}

uint8_t persist_read(uint32_t address, uint8_t* data, uint16_t length){
	eeprom_read_block(data, (void*) (uint16_t) address, length);
	return 0;
}
