#ifndef MATRIX_H
#define MATRIX_H

#ifndef MATRIX_WIDTH
#define MATRIX_WIDTH 24
#endif
#ifndef MATRIX_HEIGHT
#define MATRIX_HEIGHT 16
#endif

#ifndef MATRIX_DRIVER_SLAVE_ADDRESS
#define MATRIX_DRIVER_SLAVE_ADDRESS 42
#endif

#define MATRIX_LENGTH MATRIX_WIDTH * MATRIX_HEIGHT

//Bit depths per channel
#define MATRIX_MODE_4BIT	0x00
#define MATRIX_MODE_2BIT	0x01
#define MATRIX_MODE_1BIT	0x02	//TODO Implement

#include "../draw.h"
#include "../../twi/twi.h"

/*************
 * IMPORTANT *
 *************
 *
 * You must include a driver, which includes the following API functions.
 */

/*
 * Initializes the code to write to the matrix driver; specifically, this sets up the TWI hardware.  If you are using 
 * the TWI library elsewhere you can skip this step.
 */
void matrix_init();

 /*
 * Writes the buffer to the LED matrix, using the TWI library.
 */
void matrix_write_buffer();

/*
 * Sets the mode of the matrix driver.  Default is 0x00 (raw buffer copy).  Other modes include
 * 4 bit gradient, 4 bit high contrast, 2 bit modes of various color combinations, and 1 bit modes
 * of various combinations.  See matrix driver code for details.
 */
void matrix_set_mode(uint8_t new_mode);

/*
 * Raw access to the working buffer.  Use at your own risk.
 */
uint8_t* matrix_get_buffer();

/*
 * Finished API methods
 */

#endif