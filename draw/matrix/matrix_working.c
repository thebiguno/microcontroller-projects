/*
 * The software driver implementation for the Digital Cave LED Matrix driver board (13 chained shift registers).  This will implement
 * the API functions declared in matrix.h.
 */


#include "matrix.h"
#include <util/delay.h>

//Data is copies to the working buffer first, and then flushed to _buffer for display by the user.  This
// prevents flickering when doing animations (and in fact any live drawing).
static uint8_t _working_buffer[MATRIX_WIDTH][MATRIX_HEIGHT >> 1];

void set_pixel(uint8_t x, uint8_t y, uint8_t value, uint8_t overlay){
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return;	//Bounds check
	//Value is a 4 bit value of RG, with R as the 2 MSB and G as the 2 LSB.  Any bits over the 4 LSB are ignored.
	value = value & 0xF;	//Discard extra stuff
	if (y & 0x01) value = value << 4;	//If this is an odd row, then it will reside in the 4 MSB in the buffer; otherwise 4 LSB
	if (overlay == OVERLAY_OR){
		_working_buffer[x][y >> 1] |= value;		//Set the value
	}
	else if (overlay == OVERLAY_NAND){
		_working_buffer[x][y >> 1] &= ~value;		//Set the value
	}
	else if (overlay == OVERLAY_XOR){
		_working_buffer[x][y >> 1] ^= value;		//Set the value
	}
}

uint8_t get_pixel(uint8_t x, uint8_t y){
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return 0;	//Bounds check
	//If we are on an even y row, then the buffer's 4 LSB are the value; otherwise it is the buffer's 4 MSB.
	uint8_t ret = _working_buffer[x][y >> 1];
	ret = ret & ((y & 0x01) == 0x00 ? 0x0F : 0xF0);
	if (y & 0x01) ret = ret >> 0x04;
	return ret;
}

uint8_t* matrix_get_working_buffer(){
	return (uint8_t*) _working_buffer;
}