#include "eeprom.h"

void eeprom_write_c(uint16_t address, uint8_t data){
	//Wait for previous write to complete
	while(EECR & (1 << EEPE));

	//Set up address and data
	EEAR = address;
	EEDR = data;

	//Enable eeprom write
	EECR |= (1 << EEMPE); 
	//Start eeprom write
	EECR |= (1 << EEPE);	
}

uint8_t eeprom_read_c(uint16_t address){
	//Wait for previous write to complete
	while(EECR & (1 << EEPE));

	//Set up address
	EEAR = address;
	
	//Perform eeprom read
	EECR |= (1 << EERE);
	
	return EEDR;
}

