#include "lib/draw/matrix/matrix.h"
#include "lib/draw/draw.h"
#include "lib/twi/twi.h"

#include <util/delay.h>
#include <avr/interrupt.h>

//The buffer contains 2 bit color values for R and G channels.  The LSB 4 bits are R, MSB 4 bits are G.
// Each 'pixel' on the display (comprising a RG tuple) therefore takes up 8 bits in the buffer.
static uint8_t _buffer[MATRIX_WIDTH][MATRIX_HEIGHT];

//ShiftRegister object
static ShiftRegister shift(13);			//TODO change the size to be dynamically calculated based on width and height values

//We assume the *data is a pointer to the data we need to fill; thus data[0] is the LSB 
// shift register, and data[1] is the MSB.
static void _fill_data(uint8_t* data, uint8_t x, uint8_t y, uint8_t dc){
	//Clear data initially; we will set individual bits on as needed
	data[0] = 0x00;
	data[1] = 0x00;
	
	//Do common math once to keep things as fast as possible
	uint8_t* b = _buffer[x] + y;
	uint8_t gdc = dc << 4;	//dc for green (shift dc once, rather than green many times)
	
	//By checking for non-zero values in b[x], we can potentially reduce the time for a large number of 
	// updates for blank / sparse rows.
//	if (b[0]){
		if (b[1] > gdc)				data[1] |= 0x01;
		if ((b[1] & 0x0F) > dc)		data[1] |= 0x02;
		if (b[0] > gdc)				data[1] |= 0x40;
		if ((b[0] & 0x0F) > dc)		data[1] |= 0x80;
//	}
//	if (b[4]){
		if ((b[5] & 0x0F) > dc)		data[1] |= 0x04;
		if (b[5] > gdc)				data[1] |= 0x08;
		if ((b[4] & 0x0F) > dc)		data[1] |= 0x10;
		if (b[4] > gdc)				data[1] |= 0x20;
//	}

//	if (b[2]){
		if (b[3] > gdc)				data[0] |= 0x01;
		if ((b[3] & 0x0F) > dc)		data[0] |= 0x02;
		if (b[2] > gdc)				data[0] |= 0x40;
		if ((b[2] & 0x0F) > dc)		data[0] |= 0x80;
//	}
//	if (b[6]){
		if ((b[7] & 0x0F) > dc)		data[0] |= 0x04;
		if (b[7] > gdc)				data[0] |= 0x08;
		if ((b[6] & 0x0F) > dc)		data[0] |= 0x10;
		if (b[6] > gdc)				data[0] |= 0x20;
//	}
}

static void _callback(){
	sei();	//This is a very long ISR handler... let it be interrupted
	
	PORTB ^= _BV(PORTB0);
	
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

	DDRB |= _BV(PORTB0);

	shift.setLatch(&PORTB, PORTB2);
	shift.setCallback(_callback);
	
	_callback();	//Start shifting
	
	_buffer[0][0] = 0xFF;
	_buffer[23][0] = 0xFF;
	_buffer[0][15] = 0xFF;
	_buffer[23][15] = 0xFF;
	
	while (1) {
		//Nothing to see here... all the work is done in ISRs
	}
}
