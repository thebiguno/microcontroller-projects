#include "lib/draw/matrix/matrix.h"
#include "lib/draw/draw.h"
#include "lib/twi/twi.h"
#include "lib/ShiftRegister/ShiftRegister.h"

#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#ifndef MATRIX_DRIVER_ADDRESS
#define MATRIX_DRIVER_ADDRESS 42
#endif

static uint8_t buffer[MATRIX_WIDTH * MATRIX_HEIGHT];
//457 bytes free min.

//ShiftRegister object
static ShiftRegister shift(((MATRIX_WIDTH * MATRIX_HEIGHT >> 0) >> 5) + 1);

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
	fill_data(&data[0x00], buffer + (0x07 - row) * MATRIX_HEIGHT + 0x08, dc);
	fill_data(&data[0x02], buffer + (0x0F - row) * MATRIX_HEIGHT + 0x08, dc);
	fill_data(&data[0x04], buffer + (0x17 - row) * MATRIX_HEIGHT + 0x08, dc);
	fill_data(&data[0x06], buffer + (0x07 - row) * MATRIX_HEIGHT, dc);
	fill_data(&data[0x08], buffer + (0x0F - row) * MATRIX_HEIGHT, dc);
	fill_data(&data[0x0A], buffer + (0x17 - row) * MATRIX_HEIGHT, dc);

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

//This is called for each byte that is received.  'data' is the byte read from TWI; 'i' is the index of the data in the transmission
void slave_rx_reader(uint8_t data, uint16_t i){
	static uint8_t mode = 0x00;
	static uint16_t max_length = 0x00;
	//The first byte is mode; we copy the remaining bytes as follows:
	// 0x00: 8 bit color (blind copy of rx buffer data to internal buffer)
	// 0x01: 4 bit gradient mode
	// 0x02: 4 bit high contrast mode
	// 0x03: 2 bit, two color mode (R, Y, G, B, bright color)
	// 0x04: 2 bit, two color mode (R, Y, G, B, dim color)
	// 0x05: 2 bit green mode
	// 0x06: 2 bit yellow mode
	// 0x07: 2 bit red mode
	// 0x08: 1 bit green mode
	// 0x09: 1 bit yellow mode
	// 0x0A: 1 bit red mode
	
	if (i == 0){
		//Set the mode for this write operation
		mode = data;

		//Determine the max buffer length (bounds checking, prevent buffer overflows if the master sends too much / wrong mode / etc)
		switch (mode){
			case 0x00: 
				max_length = MATRIX_WIDTH * MATRIX_HEIGHT;
				break;
			case 0x01: 
			case 0x02: 
				max_length = MATRIX_WIDTH * (MATRIX_HEIGHT >> 1);
				break;
			case 0x03: 
			case 0x04: 
			case 0x05: 
			case 0x06: 
			case 0x07: 
				max_length = MATRIX_WIDTH * (MATRIX_HEIGHT >> 2);
				break;
			case 0x08: 
			case 0x09: 
			case 0x0A: 
				max_length = MATRIX_WIDTH * (MATRIX_HEIGHT >> 3);
				break;
		}
	}
	else if (i <= max_length){	//Verify that the incoming byte will fit in the buffer, as determined by mode
		i--;
		switch(mode){
			//8 bit mode; raw copy of all values
			case 0x00:
				buffer[i] = data;
				break;

			//4 bit gradient mode
			case 0x01:
				for (uint8_t j = 0; j < 2; j++){
					uint8_t v = (data >> (j * 2)) & 0x03;
					switch (v){
						case 0x00: buffer[i * 2 + j] = 0x00; break;
						case 0x01: buffer[i * 2 + j] = 0x01; break;
						case 0x02: buffer[i * 2 + j] = 0x02; break;
						case 0x03: buffer[i * 2 + j] = 0x04; break;
						case 0x04: buffer[i * 2 + j] = 0x08; break;
						case 0x05: buffer[i * 2 + j] = 0x0F; break;
						case 0x06: buffer[i * 2 + j] = 0x2F; break;
						case 0x07: buffer[i * 2 + j] = 0x4F; break;
						case 0x08: buffer[i * 2 + j] = 0xFF; break;
						case 0x09: buffer[i * 2 + j] = 0xF4; break;
						case 0x0A: buffer[i * 2 + j] = 0xF2; break;
						case 0x0B: buffer[i * 2 + j] = 0xF0; break;
						case 0x0C: buffer[i * 2 + j] = 0x80; break;
						case 0x0D: buffer[i * 2 + j] = 0x40; break;
						case 0x0E: buffer[i * 2 + j] = 0x20; break;
						case 0x0F: buffer[i * 2 + j] = 0x10; break;
					}
				}
				break;

			//4 bit high contrast mode
			case 0x02:
				for (uint8_t j = 0; j < 2; j++){
					uint8_t v = (data >> (j * 2)) & 0x03;
					switch (v){
						case 0x00: buffer[i * 2 + j] = 0x00; break;
						case 0x01: buffer[i * 2 + j] = 0x01; break;
						case 0x02: buffer[i * 2 + j] = 0x02; break;
						case 0x03: buffer[i * 2 + j] = 0x04; break;
						case 0x04: buffer[i * 2 + j] = 0x08; break;
						case 0x05: buffer[i * 2 + j] = 0x11; break;
						case 0x06: buffer[i * 2 + j] = 0x22; break;
						case 0x07: buffer[i * 2 + j] = 0x44; break;
						case 0x08: buffer[i * 2 + j] = 0x88; break;
						case 0x09: buffer[i * 2 + j] = 0xFF; break;
						case 0x0A: buffer[i * 2 + j] = 0x10; break;
						case 0x0B: buffer[i * 2 + j] = 0x20; break;
						case 0x0C: buffer[i * 2 + j] = 0x40; break;
						case 0x0D: buffer[i * 2 + j] = 0x60; break;
						case 0x0E: buffer[i * 2 + j] = 0x90; break;
						case 0x0F: buffer[i * 2 + j] = 0xF0; break;
					}
				}
				break;

			//2 bit three color bright mode
			case 0x03:
				for (uint8_t j = 0; j < 4; j++){
					uint8_t v = (data >> (j * 2)) & 0x03;
					switch (v){
						case 0x00: buffer[i * 4 + j] = 0x00; break;
						case 0x01: buffer[i * 4 + j] = 0x0F; break;
						case 0x02: buffer[i * 4 + j] = 0xFF; break;
						case 0x03: buffer[i * 4 + j] = 0xF0; break;
					}
				}
				break;
				

			//2 bit three color dim mode
			case 0x04:
				for (uint8_t j = 0; j < 4; j++){
					uint8_t v = (data >> (j * 2)) & 0x03;
					switch (v){
						case 0x00: buffer[i * 4 + j] = 0x00; break;
						case 0x01: buffer[i * 4 + j] = 0x04; break;
						case 0x02: buffer[i * 4 + j] = 0x44; break;
						case 0x03: buffer[i * 4 + j] = 0x40; break;
					}
				}
				break;
				


			//2 bit green
			case 0x05:
				for (uint8_t j = 0; j < 4; j++){
					uint8_t v = (data >> (j * 2)) & 0x03;
					switch (v){
						case 0x00: buffer[i * 4 + j] = 0x00; break;
						case 0x01: buffer[i * 4 + j] = 0x20; break;
						case 0x02: buffer[i * 4 + j] = 0x60; break;
						case 0x03: buffer[i * 4 + j] = 0xF0; break;
					}
				}
				break;
				
			//2 bit yellow
			case 0x06:
				for (uint8_t j = 0; j < 4; j++){
					uint8_t v = (data >> (j * 2)) & 0x03;
					switch (v){
						case 0x00: buffer[i * 4 + j] = 0x00; break;
						case 0x01: buffer[i * 4 + j] = 0x22; break;
						case 0x02: buffer[i * 4 + j] = 0x66; break;
						case 0x03: buffer[i * 4 + j] = 0xFF; break;
					}
				}
				break;
				
			//2 bit red
			case 0x07:
				for (uint8_t j = 0; j < 4; j++){
					uint8_t v = (data >> (j * 2)) & 0x03;
					switch (v){
						case 0x00: buffer[i * 4 + j] = 0x00; break;
						case 0x01: buffer[i * 4 + j] = 0x02; break;
						case 0x02: buffer[i * 4 + j] = 0x06; break;
						case 0x03: buffer[i * 4 + j] = 0x0F; break;
					}
				}
				break;

			//1 bit green
			case 0x08:
				for (uint8_t j = 0; j < 8; j++){
					buffer[i * 8 + j] = (data & _BV(j)) ? 0xF0 : 0x00;
				}
				break;
				
			//1 bit yellow
			case 0x09:
				for (uint8_t j = 0; j < 8; j++){
					buffer[i * 8 + j] = (data & _BV(j)) ? 0xFF : 0x00;
				}
				break;
				
			//1 bit red
			case 0x0A:
				for (uint8_t j = 0; j < 8; j++){
					buffer[i * 8 + j] = (data & _BV(j)) ? 0x0F : 0x00;
				}
				break;

		}
	}
	
}

int main (void){
	twi_init();
	twi_set_slave_address(MATRIX_DRIVER_ADDRESS);
	twi_attach_slave_rx_reader(slave_rx_reader);
//	twi_set_rx_buffer(buffer);

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
