#include "shift.h"
#include "../display/display.h"

#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define SIZE 2

static volatile uint8_t *_latch_port = 0;
static uint8_t _latch_pin = 0;

static volatile uint8_t buffer[SIZE];

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
	
	// setup SPI (enable, master)
	SPCR = (1<<SPE) | (1<<MSTR);
}

void shift_set(uint8_t index, uint8_t value) {
	buffer[index] = value;
}

void shift_do() {
	static uint8_t i;
	
	if (i < SIZE) {
		if(!(SPSR & (1<<SPIF)));
		SPDR = buffer[i++];
	} else {
		*_latch_port &= ~_BV(_latch_pin);	
		*_latch_port |= _BV(_latch_pin);
		i = 0;
	}
}