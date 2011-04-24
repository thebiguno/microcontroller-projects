/*
 * EEPROM persist implementation 
 */
#include <util/delay.h>
#include "../persist.h"
#include "../../../lib/eeprom/eeprom.h"

#define PERSIST_EEPROM_MAGIC_NUMBER		0x42

void persist_init(){
    ;
}


uint8_t persist_write(uint8_t section, uint8_t *data, uint8_t length){
	if (section > 0x8 || length > 0x3E){ //Keep 1 byte free for checksum
		status_set(STATUS_ERR_EEPROM); //Message too long / address undefined
		_delay_ms(1000);
		status_set(0x00);
		return 0;
	}
	
	uint16_t eeprom_address = (section & 0x7) << 6;
	uint8_t checksum = PERSIST_EEPROM_MAGIC_NUMBER;
	for (uint8_t i = 0; i < length; i++){
		eeprom_write_c(eeprom_address++, data[i]);
		checksum += data[i];
	}
	eeprom_write_c(eeprom_address++, checksum);
	
	return length;
}

uint8_t persist_read(uint8_t section, uint8_t *data, uint8_t length){
	if (section > 0x8 || length > 0x3E){ //Keep 1 byte free for checksum
		status_set(STATUS_ERR_EEPROM); //Message too long / address undefined
		return 0;
	}
	
	uint16_t eeprom_address = (section & 0x7) << 6;
	uint8_t checksum = PERSIST_EEPROM_MAGIC_NUMBER;
	for (uint8_t i = 0; i < length; i++){
		data[i] = eeprom_read_c(eeprom_address++);
		checksum += data[i];
	}
	
	if (checksum != eeprom_read_c(eeprom_address++)){
		return 0;
	}
	
	return length;	
}
