/*
 * The software driver implementation for the Digital Cave LED Matrix driver board (13 chained shift registers).  This will implement
 * the API functions declared in matrix.h.
 */


#include "matrix.h"
#include "../draw.h"
#include "../../Shift/Shift.h"
#include <util/delay.h>

//The buffer contains 2 bit color values for R and G channels.  The LSB 2 bits are R, MSB 2 bits are G.
// Each 'pixel' on the display (comprising a RG tuple) therefore takes up 4 bits in the buffer.  The 
// pixels are therefore at buffer index [x][y >> 1]: even rows are the 4 LSB of this byte, and odd rows 
// are the MSB of this byte.
static uint8_t _buffer[MATRIX_WIDTH][MATRIX_HEIGHT >> 1];
static Shift shift(13);

//First index is global value, second is local brightness
static uint8_t _dc_lookup[4][16] = {
	{
		0, 1, 1, 1,		//Red
		1, 0, 0, 0,
		1, 0, 0, 0,
		1, 0, 0, 0
		//Green
	}, {
		0, 1, 2, 3,
		1, 0, 0, 0,
		2, 0, 0, 0,
		3, 0, 0, 0
	}, {
		0, 2, 4, 6,
		2, 0, 0, 0,
		4, 0, 0, 0,
		6, 0, 0, 0
	}, {
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
	uint8_t* l = _dc_lookup[3];		//TODO hardcoded as max global brightness
	
	uint8_t msb0 = b[0] >> 4;
	uint8_t msb1 = b[1] >> 4;
	uint8_t msb2 = b[2] >> 4;
	uint8_t msb3 = b[3] >> 4;
	
	if (l[msb0 & 0x0C] > dc)	data[1] |= 0x01;
	if (l[msb0 & 0x03] > dc)	data[1] |= 0x02;
	if (l[msb2 & 0x03] > dc)	data[1] |= 0x04;
	if (l[msb2 & 0x0C] > dc)	data[1] |= 0x08;
	if (l[b[2] & 0x03] > dc)	data[1] |= 0x10;
	if (l[b[2] & 0x0C] > dc)	data[1] |= 0x20;
	if (l[b[0] & 0x0C] > dc)	data[1] |= 0x40;
	if (l[b[0] & 0x03] > dc)	data[1] |= 0x80;

	if (l[msb1 & 0x0C] > dc)	data[0] |= 0x01;
	if (l[msb1 & 0xC3] > dc)	data[0] |= 0x02;
	if (l[msb3 & 0x03] > dc)	data[0] |= 0x04;
	if (l[msb3 & 0x0C] > dc)	data[0] |= 0x08;
	if (l[b[3] & 0x03] > dc)	data[0] |= 0x10;
	if (l[b[3] & 0x0C] > dc)	data[0] |= 0x20;
	if (l[b[1] & 0x0C] > dc)	data[0] |= 0x40;
	if (l[b[1] & 0x03] > dc)	data[0] |= 0x80;
}

static void _callback(){
	//TODO Restart shifting again
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
	SPCR |= _BV(SPR0);	//Slow SPI down to f/16 so we can do other calculations faster.
//	SPDR |= _BV(SPI2X);
	
	shift.setLatch(&PORTB, PORTB2);		//TODO Check hardware defines for this
	shift.setCallback(_callback);
	
	_callback();	//Start shifting
}

void set_pixel(uint8_t x, uint8_t y, uint8_t value, uint8_t overlay){
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

uint8_t get_pixel(uint8_t x, uint8_t y){
	if (x >= MATRIX_WIDTH || y >= MATRIX_HEIGHT >> 1) return 0;
	//If we are on an even y row, then the buffer's 4 LSB are the value; otherwise it is the buffer's 4 MSB.
	uint8_t ret = _buffer[x][y >> 1];
	ret = ret & ((y & 0x01) == 0x00 ? 0x0F : 0xF0);
	if (y & 0x01) ret = ret >> 0x04;
	return ret;
}