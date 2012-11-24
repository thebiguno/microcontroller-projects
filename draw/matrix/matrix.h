#ifndef MATRIX_H
#define MATRIX_H

#ifndef MATRIX_WIDTH
#define MATRIX_WIDTH 24
#endif
#ifndef MATRIX_HEIGHT
#define MATRIX_HEIGHT 16
#endif

#include <avr/io.h>
#include "../draw.h"
#include "../../ShiftRegister/ShiftRegister.h"

/*************
 * IMPORTANT *
 *************
 *
 * You must include a driver, which includes the following API functions.
 */

 /*
 * Initialize the matrix device driver
 */
void matrix_init();

/*
 * Raw access to the working buffer.  Use at your own risk.
 */
uint8_t* matrix_get_working_buffer();

/*
 * Finished API methods
 */

#endif