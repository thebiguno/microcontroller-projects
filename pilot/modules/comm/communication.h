#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "../../main.h"

/*
 * Initializes communication hardware.
 */
void comm_init();

void comm_write(uint8_t b);

uint8_t comm_available();

uint8_t comm_read(uint8_t *b);

#endif
