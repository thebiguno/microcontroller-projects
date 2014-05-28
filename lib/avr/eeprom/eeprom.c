#include "eeprom.h"
#error "The DigitalCave EEPROM library is deprecated.  Use <avr/eeprom.h> instead of this library.  This library will be deleted in the future; please update your source code to reference the built-in EEPROM functions."

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

