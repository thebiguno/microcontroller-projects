#ifndef MATRIX_H
#define MATRIX_H

#include <avr/io.h>

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
 * Starts the ISR which refreshes the matrix display, via chained SPI shift registers
 */
void matrix_start_refreshing();

/* 
 * Starts the ISR which refreshes the matrix display, via chained SPI shift registers
 */
void matrix_stop_refreshing();


/*
 * Most basic operation; sets a pixel at the given X, Y co-ordinates.
 */
void matrix_set_pixel(uint8_t x, uint8_t y, uint8_t overlay);

/*
 * Get a a pixel at the given X, Y co-ordinates.
 */
uint8_t matrix_get_pixel(uint8_t x, uint8_t y);

/*
 * Finished API methods
 */

#endif