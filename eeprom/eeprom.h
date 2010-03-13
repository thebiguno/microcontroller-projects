#include <avr/io.h>

/*
 * Writes a data byte to eeprom at the specified address.  No bounds checking is done
 * on the address.
 */
void eeprom_write_c(uint16_t address, uint8_t data);

/*
 * Reads a data byte from eeprom at the specified address.
 */
uint8_t eeprom_read_c(uint16_t address);

