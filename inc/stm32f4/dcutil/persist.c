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

//extern UART_HandleTypeDef huart6;

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
#define FLASH_MAGIC_BYTE			(0x42)

static void persist_erase(){
	//HAL_UART_Transmit(&huart6, (uint8_t*) "Erasing...", 10, 1);
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR);
	FLASH_Erase_Sector(FLASH_SECTOR_INDEX, VOLTAGE_RANGE_3);
	HAL_FLASH_Program(TYPEPROGRAM_BYTE, (uint32_t) FLASH_BASE_ADDRESS, FLASH_MAGIC_BYTE);	//Write the magic byte
	HAL_FLASH_Lock();
	//HAL_UART_Transmit(&huart6, (uint8_t*) " Finished\n", 10, 1);
}

uint8_t persist_write(uint32_t address, uint8_t* data, uint16_t length){
	//Bounds checking
	if ((address + length) > FLASH_PAGE_SIZE) return 1;

	uint8_t oldData[FLASH_PAGE_SIZE];
	if (persist_read(0, oldData, FLASH_PAGE_SIZE)){
		//We can't read the old data; this means that we probably need to erase.
		persist_erase();
		//Reset the oldData to zeros since we can't read it.
		for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++){
			oldData[i] = 0x00;
		}
		//HAL_UART_Transmit(&huart6, (uint8_t*) "Zero data\n", 10, 1);
	}

	//Compare the old data with the new data to see if there are changes.
	uint8_t changed = 0;
	for (uint16_t i = 0; i < length; i++){
		if (oldData[address + i] != data[i]){
			oldData[address + i] = data[i];
			changed = 1;
		}
	}
	if (!changed){
		//HAL_UART_Transmit(&huart6, (uint8_t*) "No Change...\n", 13, 1);
		return 0;		//Even though we have not written anything, we consider this a success.
	}

	//Find the next available page by looking through the index page.
	uint8_t nextIndex = 0x00;
	uint8_t lastPage = 0x00;
	uint8_t nextPage = 0x00;
	for (uint16_t i = 1; i < fmin(FLASH_PAGE_SIZE, fmin(FLASH_PAGE_COUNT, 255)); i++){
		nextIndex = i;
		lastPage = nextPage;
		nextPage = *(__IO uint8_t*) (FLASH_BASE_ADDRESS + i);
		if (nextPage == 0xFF){
			nextPage = lastPage + 1;
			break;
		}
	}

	// char buffer[64];
	// uint8_t size = snprintf(buffer, sizeof(buffer), "lastPage = %02x, nextPage = %02x, nextIndex = %02x\n", lastPage, nextPage, nextIndex);
	// HAL_UART_Transmit(&huart6, (uint8_t*) buffer, size, 1);

	if (nextPage == 0xFF){
		//We are completely full; erase the sector and start over again.
		persist_erase();
		nextIndex = 0x00;
		nextPage = 0x01;
	}

	//Write the next page's data
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR);
	for (uint16_t i = 0; i < FLASH_PAGE_SIZE; i++){
		HAL_FLASH_Program(
				TYPEPROGRAM_BYTE,
				(uint32_t) FLASH_BASE_ADDRESS + (FLASH_PAGE_SIZE * nextPage) + i,
				oldData[i]
		);
	}
	// HAL_UART_Transmit(&huart6, (uint8_t*) "Wrote page\n", 11, 1);

	//Update the index with the currently written page
	HAL_FLASH_Program(
			TYPEPROGRAM_BYTE,
			(uint32_t) FLASH_BASE_ADDRESS + nextIndex,
			nextPage
	);
	// HAL_UART_Transmit(&huart6, (uint8_t*) "Wrote index\n", 12, 1);

	HAL_FLASH_Lock();

	return 0;
}

uint8_t persist_read(uint32_t address, uint8_t* data, uint16_t length){
	//Check the magic byte
	if ((*(__IO uint8_t*) FLASH_BASE_ADDRESS) != FLASH_MAGIC_BYTE){
		// HAL_UART_Transmit(&huart6, (uint8_t*) "Missing Magic...\n", 17, 1);
		return 1;
	}

	//Find the current page by looking through the first (index) page.
	uint8_t currentPage = 0x00;
	uint8_t nextPage = 0x01;
	uint8_t full = 1;	//Assume full until proven otherwise
	for (uint16_t i = 1; i < fmin(FLASH_PAGE_SIZE, fmin(FLASH_PAGE_COUNT, 255)); i++){
		currentPage = nextPage;
		nextPage = *(__IO uint8_t*) (FLASH_BASE_ADDRESS + i);
		if (nextPage == 0xFF){
			full = 0;
			break;
		}
	}

	// char buffer[64];
	// uint8_t size = snprintf(buffer, sizeof(buffer), "currentPage = %02x, nextPage = %02x\n", currentPage, nextPage);
	// HAL_UART_Transmit(&huart6, (uint8_t*) buffer, size, 1);

	if (full){
		// HAL_UART_Transmit(&huart6, (uint8_t*) "Index Full...\n", 14, 1);
		return 2;	//There is no more space left in the first page
	}
	if (currentPage == 0){
		// HAL_UART_Transmit(&huart6, (uint8_t*) "FLASH_PAGE_SIZE invalid...\n", 27, 1);
		return 3;	//Either FLASH_PAGE_SIZE or FLASH_PAGE_COUNT is invalid (less than 2)
	}

	//Look at currentPage to find the actual data
	for (uint16_t i = 0; i < length; i++){
		data[i] = *(__IO uint8_t*) (FLASH_BASE_ADDRESS + (FLASH_PAGE_SIZE * currentPage) + address + i);
	}

	return 0;
}
