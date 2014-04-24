#ifndef HEX_COMM
#define HEX_COMM

#include <avr/io.h>
#include <avr/interrupt.h>

#include "lib/universal_controller/client.h"

#define LEG_COUNT	6
#define JOINT_COUNT	2

#include "lib/serial/serial.h"

#define SPECIAL_RESET		0x01

void comm_init();

/**
 * If the result is non-zero, there is a message waiting.  Do not call comm_read() unless this is non-zero.
 */
uint8_t comm_available();

/**
 * Set the velocity, direction, and special values into the provided variables.
 */
void comm_read(double *velocity, double *direction, uint8_t *special);

#endif