#ifndef HEX_COMM
#define HEX_COMM

#include <avr/io.h>
#include <avr/interrupt.h>

#include "lib/universal_controller/client.h"
#include "lib/serial/serial.h"

#define SPECIAL_RESET		0x01

void comm_init();

/*
 * Read all buttons which have been pressed since the last
 * call to comm_read_buttons().
 */
uint16_t comm_read_buttons();

/**
 * Read the velocity + direction of desired travel
 */
void comm_read_vector(double *velocity, double *direction);

#endif