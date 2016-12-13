/*
 * WARNING:
 * This persistence layer implementation just writes bytes at the end of the flash section
 * for the currently selected chip.  This means that if your program takes up the
 * full space, PARTS OF IT WILL BE OVERWRITTEN WITH RANDOM DATA.  THIS IS A BAD THING!
 * You MUST BE SURE that you are safe to write to this area before writing.
 * The easiest / best way to ensure that you don't overwrite program data is to modify
 * the .ld script to reduce the FLASH memory areas to not include the entire last sector.
 * For instance, on the STM32F410, the last flash sector is SECTOR_4, which is 64k in length,
 * starting at address 0x8010000.  The other sectors 0 - 3 are each 16k in length, starting
 * at address 0x8000000.  This means that if you change the FLASH length to 64k, you will
 * prevent the linker from writing any data to the last sector.
 *
 * To prevent us from doing too much erasing / writing, we perform basic wear leveling
 * by using pages.  The first page is dedicated to pointing to the current page.  After that,
 * the pages are incremented in order.  Once all pages are filled, the next write will erase
 * the chip and start again.  Note that we use a maximum of 256 pages (with the first one
 * being dedicated to the index), since we use 8 bit page addressing.
 */

#include <math.h>
#include <dcutil/persist.h>
#include "stm32f4xx_hal.h"

//This is the total amount of space allowed for persistence.  This MUST be less than 1/2 the size of the FLASH_SECTOR_SIZE
// (and if you want any sort of wear leveling, it should be less than 1/3 the size).  Furthermore, the flash page size
// must be small enough to fully fit into ram, since we need to load it when doing updates / erases.
// We default the size to 256 bytes, but you can override this from the CDEFS in the makefile.
// If you make it any less than 256 bytes, you limit the total number of pages to that value.
#ifndef FLASH_PAGE_SIZE
	#define FLASH_PAGE_SIZE 		(256)
#endif

#if defined(STM32F410Rx)
	#define FLASH_SECTOR_INDEX		(FLASH_SECTOR_4)
	#define FLASH_SECTOR_SIZE		(65536)
	#define FLASH_BASE_ADDRESS		(0x8010000)
#elif defined (STM32F411xE)
	#define FLASH_SECTOR_INDEX		(FLASH_SECTOR_7)
	#define FLASH_SECTOR_SIZE		(131072)
	#define FLASH_BASE_ADDRESS		(0x8060000)
#else
	#warning Chip not defined for persistence library.  Please add flash sector size and base address.
#endif

#define FLASH_PAGE_COUNT			(FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE)

void persist_write(uint32_t address, uint8_t* data, uint16_t length){
	//Bounds checking
	if ((address + length) > FLASH_PAGE_SIZE) return;

	//Verify that data has actually changed, and update persistance array if it has
	uint8_t changed = 0;
	uint8_t updatedData[FLASH_PAGE_SIZE];
	for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++){
		updatedData[i] = *(__IO uint8_t*) (FLASH_BASE_ADDRESS + i);
	}
	for (uint16_t i = address; i < length; i++){
		if (updatedData[i] != data[i]){
			updatedData[i] = data[i];
			changed = 1;
		}
	}
	if (!changed) return;

	//Find the next available page by looking through the first page.
	uint8_t nextIndex = 0x00;
	uint8_t lastPage = 0x00;
	uint8_t nextPage = 0x01;
	for (uint16_t i = 0; i < fmin(FLASH_PAGE_SIZE, fmin(FLASH_PAGE_COUNT, 255)); i++){
		nextIndex = i;
		lastPage = nextPage;
		nextPage = *(__IO uint8_t*) (FLASH_BASE_ADDRESS + i);
		if (nextPage == 0xFF){
			nextPage = lastPage + 1;
			break;
		}
	}

	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR);

	if (nextPage == 0xFF){
		//We are completely full; erase the sector and start over again.
		FLASH_Erase_Sector(FLASH_SECTOR_INDEX, VOLTAGE_RANGE_3);
		nextIndex = 0x00;
		nextPage = 0x01;
	}

	//Write the next page's data
	for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++){
		HAL_FLASH_Program(
				TYPEPROGRAM_BYTE,
				(uint32_t) FLASH_BASE_ADDRESS + (FLASH_PAGE_SIZE * nextPage) + i,
				updatedData[i]
		);
	}

	//Update the index with the currently written page
	HAL_FLASH_Program(
			TYPEPROGRAM_BYTE,
			(uint32_t) FLASH_BASE_ADDRESS + nextIndex,
			nextPage
	);

	HAL_FLASH_Lock();
}

void persist_read(uint32_t address, uint8_t* data, uint16_t length){
	//Find the current page by looking through the first (index) page.
	uint8_t currentPage = 0x00;
	uint8_t nextPage = 0x01;
	uint8_t valid = 0;
	for (uint16_t i = 0; i < fmin(FLASH_PAGE_SIZE, fmin(FLASH_PAGE_COUNT, 255)); i++){
		currentPage = nextPage;
		nextPage = *(__IO uint8_t*) (FLASH_BASE_ADDRESS + i);
		if (nextPage == 0xFF){
			valid = 1;
			break;
		}
	}

	if (!valid || currentPage == 0x00) {
		return;			//Either the pages are not set up properly, or no data pages have been written yet.
	}

	//Look at currentPage to find the actual data
	for (uint16_t i = 0; i < length; i++){
		data[i] = *(__IO uint8_t*) (FLASH_BASE_ADDRESS + (FLASH_PAGE_SIZE * currentPage) + address + i);
	}
}
