#include "eeprom.h"

void eeprom_write_c(uint16_t address, uint8_t data){
	//Wait for previous write to complete
	while(EECR & _BV(EEPE));

	//Set up address and data
	EEAR = address;
	EEDR = data;

	//Enable eeprom write
	EECR |= _BV(EEMPE); 
	//Start eeprom write
	EECR |= _BV(EEPE);	
}

uint8_t eeprom_read_c(uint16_t address){
	//Wait for previous write to complete
	while(EECR & _BV(EEPE));

	//Set up address
	EEAR = address;
	
	//Perform eeprom read
	EECR |= _BV(EERE);
	
	return EEDR;
}

