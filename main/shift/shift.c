#include "shift.h"

#include <avr/sfr_defs.h>
#include <avr/io.h>

static volatile uint8_t *_latch_port = 0;
static uint8_t _latch_pin = 0;

static uint8_t _i = 0;
static uint8_t _buf[5];

void shift_init(
		volatile uint8_t *data_port, uint8_t data_pin, 
		volatile uint8_t *clock_port, uint8_t clock_pin, 
		volatile uint8_t *latch_port, uint8_t latch_pin) {
			
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
	*latch_port &= ~_BV(latch_pin);
	
	// init the buffer to 0
	for (uint8_t i = 0; i < 5; i++) {
		_buf[i++] = 0;
	}
	
	#ifndef NO_INTERRUPT_ENABLE
		sei();
	#endif
}

void shift_data(uint8_t data, uint8_t i) {
	_buf[i] = data;
}

void shift_enable() {
	// prime the pump
	SPDR = 0;
	while(!(SPSR & (1<<SPIF)));
	*_latch_port &= ~_BV(_latch_pin);	
	*_latch_port |= _BV(_latch_pin);
}

ISR(SPI_STC_vect) {
	SPDR = _buf[_i++];
	if (_i > 4) {
		_i = 0;
		*_latch_port &= ~_BV(_latch_pin);	
		*_latch_port |= _BV(_latch_pin);
	}
}