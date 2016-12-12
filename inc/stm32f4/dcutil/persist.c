#include <dcutil/persist.h>


void persist_write(uint32_t address, uint8_t* data, uint16_t length){
	uint8_t flash_status = FLASH_COMPLETE;

	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR
		| FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERRFLASH_FLAG_EOP
		| FLASH_FLAG_OPERR |FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR
	 	| FLASH_FLAG_PGSERR);

		FLASH_SECTOR_0
	//eeprom_update_block(data, (void*) (uint16_t) address, length);
}

void persist_read(uint32_t address, uint8_t* data, uint16_t length){
	//eeprom_read_block(data, (void*) (uint16_t) address, length);
}
