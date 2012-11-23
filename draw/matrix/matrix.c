/*
 * The software driver implementation for the Digital Cave LED Matrix driver board (13 chained shift registers).  This will implement
 * the API functions declared in matrix.h.
 */


#include "matrix.h"
#include <util/delay.h>

//Data is copies to the working buffer first, and then flushed to _buffer for display by the user.  This
// prevents flickering when doing animations (and in fact any live drawing).
static uint8_t _working_buffer[MATRIX_WIDTH][MATRIX_HEIGHT];

void set_pixel(uint8_t x, uint8_t y, uint8_t value, uint8_t overlay){
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return;	//Bounds check

	if (overlay == OVERLAY_OR){
		_working_buffer[x][y] |= value;		//Set the value
	}
	else if (overlay == OVERLAY_NAND){
		_working_buffer[x][y] &= ~value;		//Set the value
	}
	else if (overlay == OVERLAY_XOR){
		_working_buffer[x][y] ^= value;		//Set the value
	}
}

uint8_t get_pixel(uint8_t x, uint8_t y){
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return 0;	//Bounds check
	return _working_buffer[x][y];
}

uint8_t* matrix_get_working_buffer(){
	return (uint8_t*) _working_buffer;
}