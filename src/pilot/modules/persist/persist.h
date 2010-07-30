#ifndef PERSIST_H
#define PERSIST_H

#include "../../main.h"

#define PERSIST_SECTION_ACCEL 0x0

/*
 * Initializes persistence hardware
 */
void persist_init();

/*
 * Persists data to address.  Section is the module name, and is defined above. 
 * Implementations may not use the entire 8 bits for section and / or address,
 * in which case only valid addresses / sections are used.  Returns the total 
 * number of bytes written successfully.
 */
uint8_t persist_write(uint8_t section, uint8_t address, uint8_t *data, uint8_t length);

/*
 * Reads data from address.  Section is the module name, as defined above.  Returns
 * the number of bytes actually read.
 */
uint8_t persist_read(uint8_t section, uint8_t address, uint8_t *data, uint8_t length);

#endif

