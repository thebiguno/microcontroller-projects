#include "shift.h"

#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// 5 shift registers and 64 bytes for each color (8 rows * 8 columns)
#define SIZE 5
#define DEPTH 128

static volatile uint8_t *_latch_port = 0;
static uint8_t _latch_pin = 0;

static volatile uint8_t i;
static volatile uint8_t j;
static volatile uint8_t _buffer[SIZE][DEPTH]; // marked as volatile because it's used in the IVR

void shift_init(volatile uint8_t *data_port, uint8_t data_pin, volatile uint8_t *clock_port, uint8_t clock_pin, volatile uint8_t *latch_port, uint8_t latch_pin){
	_latch_port = latch_port;
	_latch_pin = latch_pin;

	// set ddr output
	*(data_port - 0x1) |= _BV(data_pin);
	*(clock_port - 0x1) |= _BV(clock_pin);
	*(latch_port - 0x1) |= _BV(latch_pin);
	
	// pull pins low
	*data_port &= ~_BV(data_pin);
	*clock_port &= ~_BV(clock_pin);
	*latch_port &= ~_BV(latch_pin);
	
	// setup SPI (enable, master, interrupt)
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPIE);
	
	// pull LATCH low
	//*latch_port &= ~_BV(latch_pin);
	
	#ifndef NO_INTERRUPT_ENABLE
	sei();
	#endif
}

void shift_enable() {
	// prime the pump with garbage to get the ISR going
	SPDR = 0;
	// wait for spi process to finish
	while(!(SPSR & (1<<SPIF)));
		// *_latch_port &= ~_BV(_latch_pin);
		// *_latch_port |= _BV(_latch_pin);
}

void shift_data(uint8_t i, uint8_t j, uint8_t data) {
	_buffer[i][j] = data;
}

ISR(SPI_STC_vect) {
	SPDR = _buffer[i++][j];
	if (i == SIZE) {
		i = 0;
		*_latch_port &= ~_BV(_latch_pin);	
		*_latch_port |= _BV(_latch_pin);

		j++;
		if (j == 8) j = 0;
	}		
}