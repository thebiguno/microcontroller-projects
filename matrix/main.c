#include "lib/draw/matrix/matrix.h"
#include "lib/draw/draw.h"
#include "lib/twi/twi.h"

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

//The buffer contains 2 bit color values for R and G channels.  The LSB 4 bits are R, MSB 4 bits are G.
// Each 'pixel' on the display (comprising a RG tuple) therefore takes up 8 bits in the buffer.
static uint8_t _buffer[MATRIX_WIDTH][MATRIX_HEIGHT];

//ShiftRegister object
static ShiftRegister shift(((MATRIX_WIDTH * MATRIX_HEIGHT) >> 5) + 1);

//We assume the *data is a pointer to the data we need to fill; thus data[0] is the LSB 
// shift register, and data[1] is the MSB.
void fill_data(uint8_t* data, uint8_t* b, const register uint8_t dc) {
	
	//Do common math once to keep things as fast as possible
	const register uint8_t gdc = dc << 4;	//dc for green (shift dc once, rather than green many times)

	register uint8_t d0 = 0x00;
	register uint8_t d1 = 0x00;

	if (b[0] > gdc)				d1 |= 0x40;
	if ((b[0] & 0x0F) > dc)		d1 |= 0x80;
	if (b[1] > gdc)				d1 |= 0x01;
	if ((b[1] & 0x0F) > dc)		d1 |= 0x02;

	if (b[2] > gdc)				d0 |= 0x40;
	if ((b[2] & 0x0F) > dc)		d0 |= 0x80;
	if (b[3] > gdc)				d0 |= 0x01;
	if ((b[3] & 0x0F) > dc)		d0 |= 0x02;

	if (b[4] > gdc)				d1 |= 0x20;
	if ((b[4] & 0x0F) > dc)		d1 |= 0x10;
	if (b[5] > gdc)				d1 |= 0x08;
	if ((b[5] & 0x0F) > dc)		d1 |= 0x04;

	if (b[6] > gdc)				d0 |= 0x20;
	if ((b[6] & 0x0F) > dc)		d0 |= 0x10;
	if (b[7] > gdc)				d0 |= 0x08;
	if ((b[7] & 0x0F) > dc)		d0 |= 0x04;
	
	//By using temp variables we avoid a lot of dereferencing.  Yay!
	data[0] = d0;
	data[1] = d1;
}

static void _callback(){
	sei();	//This is a very long ISR handler... let it be interrupted
	
	PORTB ^= _BV(PORTB0);
	
	static uint8_t data[13];
	static uint8_t row = 0;
	static uint8_t dc = 0;
	
	//Fill the data array based on current duty cycle, row, and buffer values
	fill_data(&data[0x00], &_buffer[0x07 - row][0x08], dc);
	fill_data(&data[0x02], &_buffer[0x0F - row][0x08], dc);
	fill_data(&data[0x04], &_buffer[0x17 - row][0x08], dc);
	fill_data(&data[0x06], &_buffer[0x07 - row][0x00], dc);
	fill_data(&data[0x08], &_buffer[0x0F - row][0x00], dc);
	fill_data(&data[0x0A], &_buffer[0x17 - row][0x00], dc);

	//Set row driver
	data[0x0C] = ~_BV(row++);
	if (row > 7){
		row = 0;
		dc++;
		if (dc > 15) dc = 0;
	}
	
	wdt_reset();
	
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
	wdt_enable(WDTO_15MS);

	DDRB |= _BV(PORTB0);

	//Make SPI as fast as possible (fcpu / 2)
	SPDR |= _BV(SPI2X);

	shift.setLatch(&PORTB, PORTB2);
	shift.setCallback(_callback);
	
	_callback();	//Start shifting

	while (1) {
		//Nothing to see here... all the work is done in ISRs
	}
}
