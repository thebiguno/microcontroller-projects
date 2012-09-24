/*
 * The software driver implementation for the Digital Cave LED Matrix driver board (13 chained shift registers).  This will implement
 * the API functions declared in matrix.h.
 */


#include "matrix.h"


//The buffer 
static uint8_t _buffer[MATRIX_WIDTH][MATRIX_HEIGHT >> 3];
static Shift shift(13);

void _callback(){
	//TODO Restart shifting again
	static uint8_t data[13];
	static uint8_t row = 0;
	
	//Set row driver
	data[12] = ~_BV(row++);
	if (row > 7) row = 0;
	
	
}

void matrix_init(){
	shift.setLatch(&PORTB, PORTB2);		//TODO Check hardware defines for this
	shift.setCallback(_callback);
	
	_callback();	//Start shifting
}

void set_pixel(uint8_t x, uint8_t y, uint8_t value, uint8_t overlay){
	//Value is a 4 bit value of RG, with R as the 2 MSB and G as the 2 LSB.  Any bits over the 4 LSB are ignored.
}

uint8_t matrix_get_pixel(uint8_t x, uint8_t y){

}