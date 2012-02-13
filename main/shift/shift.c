#include "shift.h"

#include <avr/sfr_defs.h>
#include <avr/io.h>

static volatile uint8_t *_latch_port = 0;
static uint8_t _latch_pin = 0;

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
	
	// setup SPI
	SPCR = (1<<SPE) | (1<<MSTR);
	
	// pull LATCH low
	*latch_port &= ~_BV(latch_pin);
}

void shift_data(uint8_t data) {
	SPDR = data;
	// wait for spi process to finish
	while(!(SPSR & (1<<SPIF)));
}

void shift_latch() {
	*_latch_port &= ~_BV(_latch_pin);	
	*_latch_port |= _BV(_latch_pin);
}