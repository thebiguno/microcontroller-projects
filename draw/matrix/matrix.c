/*
 * The software driver implementation for the Digital Cave LED Matrix driver board (13 chained shift registers).  This will implement
 * the API functions declared in matrix.h.
 */


#include "matrix.h"
#include <util/delay.h>

//Data is copies to the working buffer first, and then flushed to _buffer for display by the user.  This
// prevents flickering when doing animations (and in fact any live drawing).
static uint8_t buffer[MATRIX_WIDTH][MATRIX_HEIGHT];
static uint8_t mode = 0x00;

static uint8_t twi_master_tx_writer(uint16_t i){
	if (i == 0){
		return mode;
	}
	else {
		uint8_t result = 0x00;
		if (mode == MATRIX_MODE_4BIT) {
			// return the byte as is
			result = ((uint8_t*) buffer)[i-1];
		} else if (mode == MATRIX_MODE_2BIT) {
			uint16_t idx = (i - 1) * 2;
			//xxxxggrr xxxxggrr -> ggrrggrr
			//combine two bytes in the buffer into on byte in the message
			result = (((uint8_t*) buffer)[idx] << 4) & 0xC0; // bits 6,7
			result |= (((uint8_t*) buffer)[idx] << 4) & 0x30; // bits 4,5
			idx++;
			result |= ((uint8_t*) buffer)[idx] & 0x0C; // bits 2,3
			result |= ((uint8_t*) buffer)[idx] & 0x03; // bits 0,1
			// result = 0xc4;
		} else if (mode == MATRIX_MODE_1BIT) {
//			result = 0x33;
		}
		return result;
	}
}

void matrix_init(){
	twi_init();
	twi_attach_master_tx_writer(twi_master_tx_writer);
}

void set_pixel(int16_t x, int16_t y, uint8_t value, uint8_t overlay){
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT || x < 0 || y < 0) return;	//Bounds check

	if (overlay == OVERLAY_REPLACE){
		buffer[x][y] = value;
	}
	else if (overlay == OVERLAY_OR){
		buffer[x][y] |= value;
	}
	else if (overlay == OVERLAY_NAND){
		buffer[x][y] &= ~value;
	}
	else if (overlay == OVERLAY_XOR){
		buffer[x][y] ^= value;
	}
}

void matrix_write_buffer(){
	uint16_t l = MATRIX_LENGTH;
	if (mode == 0x01) l = l >> 1;
	else if (mode == 0x02) l = l >> 2;
	l++;
	twi_write_to(MATRIX_DRIVER_SLAVE_ADDRESS, (uint8_t*) buffer, l, TWI_BLOCK, TWI_STOP);
}

void matrix_set_mode(uint8_t new_mode){
	mode = new_mode;
}

uint8_t get_pixel(int16_t x, int16_t y){
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT || x < 0 || y < 0) return 0;	//Bounds check
	return buffer[x][y];
}

uint8_t* matrix_get_buffer(){
	return (uint8_t*) buffer;
}