#ifndef HEX_COMM
#define HEX_COMM

#include <avr/io.h>
#include <avr/interrupt.h>

#include "lib/universal_controller/client.h"
#include "lib/serial/serial.h"

#define SPECIAL_RESET		0x01

void comm_init();

/**
 * Set the velocity, direction, and special values into the provided variables.
 */
void comm_read(double *velocity, double *direction);

/**
 * Return non-zero if reset has been pushed.
 */
uint8_t comm_read_reset();

/**
 * Resets special variable
 */
void comm_reset_special();

#endif