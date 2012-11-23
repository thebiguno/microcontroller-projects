#include "lib/draw/matrix/matrix.h"
#include "lib/draw/draw.h"
#include "lib/twi/twi.h"

#include <util/delay.h>
#include <avr/interrupt.h>

//The buffer contains 2 bit color values for R and G channels.  The LSB 2 bits are R, MSB 2 bits are G.
// Each 'pixel' on the display (comprising a RG tuple) therefore takes up 4 bits in the buffer.  The 
// pixels are therefore at buffer index [x][y >> 1]: even rows are the 4 LSB of this byte, and odd rows 
// are the MSB of this byte.
static uint8_t _buffer[MATRIX_WIDTH][MATRIX_HEIGHT >> 1];

//ShiftRegister object
static ShiftRegister shift(13);			//TODO change the size to be dynamically calculated based on width and height values

//First index is global value, second is local brightness
static uint8_t _dc_lookup[4][16] = {
	{
		0, 1, 1, 1,	//Red
		1, 0, 0, 0,
		1, 0, 0, 0,
		1, 0, 0, 0
		//Green
	},
	{
		0, 1, 2, 3,	//Red
		1, 0, 0, 0,
		2, 0, 0, 0,
		3, 0, 0, 0
		//Green
	},
	{
		0, 2, 4, 6,	//Red
		2, 0, 0, 0,
		4, 0, 0, 0,
		6, 0, 0, 0
		//Green
	},
	{
		0, 1, 3, 8,	//Red
		1, 0, 0, 0,
		3, 0, 0, 0,
		8, 0, 0, 0
		//Green
	}
};

//We assume the *data is a pointer to the data we need to fill; thus data[0] is the LSB 
// shift register, and data[1] is the MSB.
static void _fill_data(uint8_t* data, uint8_t x, uint8_t y, uint8_t dc){
	//Clear data initially; we will set individual bits on as needed
	data[0] = 0x00;
	data[1] = 0x00;
	
	//Do common math once to keep things as fast as possible
	uint8_t* b = _buffer[x] + (y >> 1);
	uint8_t* l = _dc_lookup[3];		//TODO hardcoded as max global brightness
	
	//By checking for non-zero values in b[x], we can potentially reduce the time for a large number of 
	// updates for blank / sparse rows.
	if (b[0]){
		uint8_t msb0 = b[0] >> 4;
		if (l[msb0 & GRN_3] > dc)	data[1] |= 0x01;
		if (l[msb0 & RED_3] > dc)	data[1] |= 0x02;
		if (l[b[0] & GRN_3] > dc)	data[1] |= 0x40;
		if (l[b[0] & RED_3] > dc)	data[1] |= 0x80;
	}
	if (b[2]){
		uint8_t msb2 = b[2] >> 4;
		if (l[msb2 & RED_3] > dc)	data[1] |= 0x04;
		if (l[msb2 & GRN_3] > dc)	data[1] |= 0x08;
		if (l[b[2] & RED_3] > dc)	data[1] |= 0x10;
		if (l[b[2] & GRN_3] > dc)	data[1] |= 0x20;
	}

	if (b[1]){
		uint8_t msb1 = b[1] >> 4;
		if (l[msb1 & GRN_3] > dc)	data[0] |= 0x01;
		if (l[msb1 & RED_3] > dc)	data[0] |= 0x02;
		if (l[b[1] & GRN_3] > dc)	data[0] |= 0x40;
		if (l[b[1] & RED_3] > dc)	data[0] |= 0x80;
	}
	if (b[3]){
		uint8_t msb3 = b[3] >> 4;
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
	_fill_data(&data[0], 0x07 - row, 0x08, dc);
	_fill_data(&data[2], 0x0F - row, 0x08, dc);
	_fill_data(&data[4], 0x17 - row, 0x08, dc);
	_fill_data(&data[6], 0x07 - row, 0x00, dc);
	_fill_data(&data[8], 0x0F - row, 0x00, dc);
	_fill_data(&data[10], 0x17 - row, 0x00, dc);

	//Set row driver
	data[12] = ~_BV(row++);
	if (row > 7){
		row = 0;
		dc++;
		if (dc > 8) dc = 0;
	}
	
	
	shift.shift(data);
}

void flushBufferCallback(uint8_t* data, uint16_t length){
	//Do nothing... the slave rx buffer *is* the display buffer
}

int main (void){
	twi_init();
	twi_set_slave_address(42);
	twi_attach_slave_rx_event(flushBufferCallback);
	twi_set_rx_buffer((uint8_t*) _buffer);

	shift.setLatch(&PORTB, PORTB2);
	shift.setCallback(_callback);
	
	_callback();	//Start shifting
	
	while (1) {
		//Nothing to see here... all the work is done in ISRs
	}
}
