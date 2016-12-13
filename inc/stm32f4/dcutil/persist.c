/*
 * WARNING:
 * This persistence layer implementation just writes bytes at the end of the flash section
 * for the currently selected chip.  This means that if your program takes up the
 * full space, PARTS OF IT WILL BE OVERWRITTEN WITH RANDOM DATA.  THIS IS A BAD THING!
 * You MUST BE SURE that you are safe to write to this area before writing.
 *
 * TODO: Wear leveling, cross chip capabilities, etc.
 */

#include <dcutil/persist.h>
#include "stm32f4xx_hal.h"

#define FLASH_SIZE (1024)
#define FLASH_BASE_ADDRESS (0x8010000)

void persist_write(uint32_t address, uint8_t* data, uint16_t length){
	//Bounds checking
	if ((address + length) > FLASH_SIZE) return;

	//Verify that data has actually changed, and update persistance array if it has
	uint8_t changed = 0;
	uint8_t updatedData[FLASH_SIZE];
	for (uint16_t i = 0; i < FLASH_SIZE; i++){
		updatedData[i] = *(__IO uint8_t*) (FLASH_BASE_ADDRESS + i);
	}
	for (uint16_t i = address; i < length; i++){
		if (updatedData[i] != data[i]){
			updatedData[i] = data[i];
			changed = 1;
		}
	}
	if (!changed) return;

	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR);
	FLASH_Erase_Sector(FLASH_SECTOR_4, VOLTAGE_RANGE_3);
	for (uint16_t i = 0; i < FLASH_SIZE; i++){
		HAL_FLASH_Program(
				TYPEPROGRAM_BYTE,
				(uint32_t) FLASH_BASE_ADDRESS + i,
				updatedData[i]
		);
	}

	HAL_FLASH_Lock();
}

void persist_read(uint32_t address, uint8_t* data, uint16_t length){
	for (uint16_t i = 0; i < length; i++){
		data[i] = *(__IO uint8_t*) (FLASH_BASE_ADDRESS + address + i);
	}
}
