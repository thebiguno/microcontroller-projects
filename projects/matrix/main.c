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

static uint8_t dc_max = 0x00;

//ShiftRegister object
static ShiftRegister shift(((MATRIX_WIDTH * MATRIX_HEIGHT >> 0) >> 5) + 1);

//We assume the *data is a pointer to the data we need to fill; thus data[0] is the LSB 
// shift register, and data[1] is the MSB.
void fill_data(uint8_t* data, uint8_t* b, const register uint8_t dc) {
	//Do common math once to keep things as fast as possible
	const register uint8_t gdc = dc << 4;	//dc for green (shift dc once, rather than green many times)

	register uint8_t d0 = 0x00;
	register uint8_t d1 = 0x00;

	if ((b[0] & 0xF0) > gdc)	d1 |= 0x40;
	if ((b[0] & 0x0F) > dc)		d1 |= 0x80;
	if ((b[1] & 0xF0) > gdc)	d1 |= 0x01;
	if ((b[1] & 0x0F) > dc)		d1 |= 0x02;

	if ((b[2] & 0xF0) > gdc)	d0 |= 0x40;
	if ((b[2] & 0x0F) > dc)		d0 |= 0x80;
	if ((b[3] & 0xF0) > gdc)	d0 |= 0x01;
	if ((b[3] & 0x0F) > dc)		d0 |= 0x02;

	if ((b[4] & 0xF0) > gdc)	d1 |= 0x20;
	if ((b[4] & 0x0F) > dc)		d1 |= 0x10;
	if ((b[5] & 0xF0) > gdc)	d1 |= 0x08;
	if ((b[5] & 0x0F) > dc)		d1 |= 0x04;

	if ((b[6] & 0xF0) > gdc)	d0 |= 0x20;
	if ((b[6] & 0x0F) > dc)		d0 |= 0x10;
	if ((b[7] & 0xF0) > gdc)	d0 |= 0x08;
	if ((b[7] & 0x0F) > dc)		d0 |= 0x04;
	
	//By using temp variables we avoid a lot of dereferencing.  Yay!
	data[0] = d0;
	data[1] = d1;
}

static void load_shift_data(){
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
		if (dc >= dc_max) dc = 0;
	}
	
	wdt_reset();
	
	shift.shift(data);
}

//This is called for each byte that is received.  'data' is the byte read from TWI; 'i' is the index of the data in the transmission
void slave_rx_reader(uint8_t data, uint16_t i){
	static uint8_t mode = MATRIX_MODE_4BIT;
	static uint16_t max_length = 0x00;
	//The first byte is mode; we copy the remaining bytes as follows:
	// MATRIX_MODE_4BIT: 4 bit / channel mode GGGGRRRR (gives you 15 shades of red, 15 shades of green, plus black for 226 colors total, dc_max = 15)
	// MATRIX_MODE_2BIT: 2 bit / channel mode GGRR (gives you 3 shades of red, 3 shades of green, plus black for 10 colors total, dc_max = 3)
	// MATRIX_MODE_1BIT: 1 bit / channel mode GR (gives you red, green, yellow, and black pixels, dc_max = 1)
	
	if (i == 0){
		//Set the mode for this write operation
		mode = data;

		//Determine the max buffer length (bounds checking, prevent buffer overflows if the master 
		// sends too much / wrong mode / etc) and dc_max
		if (mode == MATRIX_MODE_4BIT) {
			max_length = MATRIX_WIDTH * MATRIX_HEIGHT;
			dc_max = 15;
		} else if (mode == MATRIX_MODE_2BIT) {
			max_length = MATRIX_WIDTH * (MATRIX_HEIGHT >> 1);
			dc_max = 3;
		} else {
			max_length = MATRIX_WIDTH * (MATRIX_HEIGHT >> 2);
			dc_max = 1;
		}
	} else if (i <= max_length){	//Verify that the incoming byte will fit in the buffer, as determined by mode
		i--;
		if (mode == MATRIX_MODE_4BIT) {
			//8 bit mode; raw copy of all values
			// ggggrrrr -> ggggrrrr
			buffer[i] = data;
		} else if (mode == MATRIX_MODE_2BIT) {
			//4 bit (10 color) mode
			// ggrrggrr -> xxggxxrr xxggxxrr
			uint16_t idx = i * 2;
			buffer[idx] = (data >> 4) & 0x03; // bits 4,5 for red
			buffer[idx] |= (data >> 2) & 0x30; // bits 6,7 for green
			idx++;
			buffer[idx] = (data) & 0x03; // bits 0,1 for red
			buffer[idx] |= (data << 2) & 0x30; // bits 2,3 for green
		} else {
			//2 bit (4 color) mode
				/*
				for (uint8_t j = 0; j < 4; j++){
					uint8_t v = (data >> (j * 2)) & 0x03;
					switch (v){
						case 0x00: buffer[i * 4 + j] = 0x00; break;
						case 0x01: buffer[i * 4 + j] = 0x0F; break;
						case 0x02: buffer[i * 4 + j] = 0xFF; break;
						case 0x03: buffer[i * 4 + j] = 0xF0; break;
					}
				}
				*/
		}
	}
}

int main (void){
	twi_init();
	twi_set_slave_address(MATRIX_DRIVER_ADDRESS);
	twi_attach_slave_rx_reader(slave_rx_reader);

	wdt_enable(WDTO_1S);

	//Make SPI as fast as possible (fcpu / 2)
	SPDR |= _BV(SPI2X);

	shift.setLatch(&PORTB, PORTB2);

	while (1) {
		if (shift.cts()) {
			load_shift_data();	//Start shifting
		}
	}
}
