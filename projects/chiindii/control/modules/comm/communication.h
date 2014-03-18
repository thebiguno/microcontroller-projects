#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "../../main.h"

/*
 * Initializes communication hardware.
 */
void comm_init();

void comm_pilot_write(uint8_t b);
uint8_t comm_pilot_available();
uint8_t comm_pilot_read(uint8_t *b);

void comm_pc_write(uint8_t b);
uint8_t comm_pc_available();
uint8_t comm_pc_read(uint8_t *b);

#endif
