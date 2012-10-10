#ifndef MATRIX_H
#define MATRIX_H

#ifndef MATRIX_WIDTH
#define MATRIX_WIDTH 24
#endif
#ifndef MATRIX_HEIGHT
#define MATRIX_HEIGHT 16
#endif

#define GRN_3	0xC
#define GRN_2	0x8
#define GRN_1	0x4
#define RED_3	0x3
#define RED_2	0x2
#define RED_1	0x1

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
 * Flushes the working buffer to the display buffer.  Call this after writing using the draw API and set_pixel.
 */
void matrix_flush(uint8_t* working_buffer, uint8_t* display_buffer);

/*
 * Identical to get_pixel, except it returns the pixel from the display buffer, rather than the working buffer.
 */
uint8_t matrix_get_display_pixel(uint8_t x, uint8_t y);

/*
 * Identical to set_pixel, except it sets the pixel in the display buffer, rather than the working buffer.
 */
void matrix_set_display_pixel(uint8_t x, uint8_t y, uint8_t value, uint8_t overlay);

/*
 * Raw access to the working buffer.  Use at your own risk.
 */
uint8_t* matrix_get_working_buffer();

/*
 * Raw access to the display buffer.  Use at your own risk.
 */
uint8_t* matrix_get_display_buffer();

/*
 * Finished API methods
 */

#endif