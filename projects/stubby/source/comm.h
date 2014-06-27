#ifndef HEX_COMM
#define HEX_COMM

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Point.h"
#include "lib/universal_controller/client.h"
#include "lib/serial/serial.h"

void comm_init();

/*
 * Read all buttons which have been pressed since the last
 * call to comm_read_buttons().
 */
uint16_t comm_read_buttons();

/*
 * Read the left stick (x and y).  0 is neutral, negative 
 * numbers are left / down, positive numbers are right / up.
 */
Point comm_read_left();

/*
 * Read the right stick (x and y).  0 is neutral, negative 
 * numbers are left / down, positive numbers are right / up.
 */
Point comm_read_right();

#endif