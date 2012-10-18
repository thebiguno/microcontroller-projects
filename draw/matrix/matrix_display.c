/*
 * The software driver implementation for the Digital Cave LED Matrix driver board (13 chained shift registers).  This will implement
 * the API functions declared in matrix.h.
 */


#include "matrix.h"
#include <avr/interrupt.h>

//The buffer contains 2 bit color values for R and G channels.  The LSB 2 bits are R, MSB 2 bits are G.
// Each 'pixel' on the display (comprising a RG tuple) therefore takes up 4 bits in the buffer.  The 
// pixels are therefore at buffer index [x][y >> 1]: even rows are the 4 LSB of this byte, and odd rows 
// are the MSB of this byte.
static uint8_t _buffer[MATRIX_WIDTH][MATRIX_HEIGHT >> 1];

#ifndef MATRIX_NO_SHIFT
//ShiftRegister object
static ShiftRegister shift(13);			//TODO change the size to be dynamically calculated based on width and height values

//First index is global value, second is local brightness
static uint8_t _dc_lookup[1][16] = {
	{
		0, 3, 5, 8,	//Red
		3, 0, 0, 0,
		5, 0, 0, 0,
		8, 8, 0, 0
		//Green
	}
};

//We assume the *data is a pointer to the data we need to fill; thus data[0] is the LSB 
// shift register, and data[1] is the MSB.
static void _fill_data(uint8_t* data, uint8_t x, uint8_t y, uint8_t dc){
	//Clear data initially; we will set individual bits on as needed
	data[0] = 0x00;
	data[1] = 0x00;
	
	//Do complex math once to keep things fast
	uint8_t* b = _buffer[x] + (y >> 1);
	uint8_t* l = _dc_lookup[0];		//TODO hardcoded as max global brightness
	
	//Since we need to dereference and shift, we store these locally.
	uint8_t msb0 = b[0] >> 4;
	uint8_t msb1 = b[1] >> 4;
	uint8_t msb2 = b[2] >> 4;
	uint8_t msb3 = b[3] >> 4;
	
	//By checking for non-zero values in b[x], we can potentially reduce the time for a large number of 
	// updates for blank / sparse rows.
	if (b[0]){
		if (l[msb0 & GRN_3] > dc)	data[1] |= 0x01;
		if (l[msb0 & RED_3] > dc)	data[1] |= 0x02;
		if (l[b[0] & GRN_3] > dc)	data[1] |= 0x40;
		if (l[b[0] & RED_3] > dc)	data[1] |= 0x80;
	}
	if (b[2]){
		if (l[msb2 & RED_3] > dc)	data[1] |= 0x04;
		if (l[msb2 & GRN_3] > dc)	data[1] |= 0x08;
		if (l[b[2] & RED_3] > dc)	data[1] |= 0x10;
		if (l[b[2] & GRN_3] > dc)	data[1] |= 0x20;
	}

	if (b[1]){
		if (l[msb1 & GRN_3] > dc)	data[0] |= 0x01;
		if (l[msb1 & RED_3] > dc)	data[0] |= 0x02;
		if (l[b[1] & GRN_3] > dc)	data[0] |= 0x40;
		if (l[b[1] & RED_3] > dc)	data[0] |= 0x80;
	}
	if (b[3]){
		if (l[msb3 & RED_3] > dc)	data[0] |= 0x04;
		if (l[msb3 & GRN_3] > dc)	data[0] |= 0x08;
		if (l[b[3] & RED_3] > dc)	data[0] |= 0x10;
		if (l[b[3] & GRN_3] > dc)	data[0] |= 0x20;
	}
}

static void _callback(){
	sei();	//This is a very long ISR handler... let it be interrupted
	static uint8_t data[13];
	static uint8_t row = 0;
	static uint8_t dc = 0;	//4 bit duty cycle; 2 bits brightness, 2 bits color per pixel.
	
	//Fill the data array based on current duty cycle, row, and buffer values
	_fill_data(&data[0], 0x00 + (7 - row), 0x08, dc);
	_fill_data(&data[2], 0x08 + (7 - row), 0x08, dc);
	_fill_data(&data[4], 0x10 + (7 - row), 0x08, dc);
	_fill_data(&data[6], 0x00 + (7 - row), 0x00, dc);
	_fill_data(&data[8], 0x08 + (7 - row), 0x00, dc);
	_fill_data(&data[10], 0x10 + (7 - row), 0x00, dc);

	//Set row driver
	data[12] = ~_BV(row++);
	if (row > 7){
		row = 0;
		dc++;
		if (dc > 8) dc = 0;
	}
	
	
	shift.shift(data);
}

void matrix_init(){
//	SPCR |= _BV(SPR0);	//Slow SPI down to fcpu/8 so we can do other calculations faster.
	SPDR |= _BV(SPI2X);	//SPI = fcpu / 2
	
	shift.setLatch(&PORTB, PORTB2);		//TODO Check hardware defines for this
	shift.setCallback(_callback);
	
	_callback();	//Start shifting
}
#endif

void matrix_flush(uint8_t* working_buffer, uint8_t* display_buffer){
	for (uint8_t i = 0; i < MATRIX_WIDTH * (MATRIX_HEIGHT >> 1); i++){
		display_buffer[i] = working_buffer[i];
	}
}


/*
 * Identical to get_pixel, except it returns the pixel from the display buffer, rather than the working buffer.
 */
uint8_t matrix_get_display_pixel(uint8_t x, uint8_t y){
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return 0;	//Bounds check
	//If we are on an even y row, then the buffer's 4 LSB are the value; otherwise it is the buffer's 4 MSB.
	uint8_t ret = _buffer[x][y >> 1];
	ret = ret & ((y & 0x01) == 0x00 ? 0x0F : 0xF0);
	if (y & 0x01) ret = ret >> 0x04;
	return ret;
}

void matrix_set_display_pixel(uint8_t x, uint8_t y, uint8_t value, uint8_t overlay){
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT) return;	//Bounds check
	//Value is a 4 bit value of RG, with R as the 2 MSB and G as the 2 LSB.  Any bits over the 4 LSB are ignored.
	value = value & 0xF;	//Discard extra stuff
	if (y & 0x01) value = value << 4;	//If this is an odd row, then it will reside in the 4 MSB in the buffer; otherwise 4 LSB
	if (overlay == OVERLAY_OR){
		_buffer[x][y >> 1] |= value;		//Set the value
	}
	else if (overlay == OVERLAY_NAND){
		_buffer[x][y >> 1] &= ~value;		//Set the value
	}
	else if (overlay == OVERLAY_XOR){
		_buffer[x][y >> 1] ^= value;		//Set the value
	}
}

uint8_t* matrix_get_display_buffer(){
	return (uint8_t*) _buffer;
}