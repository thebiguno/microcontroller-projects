#include "manchester.h"
#define __DELAY_BACKWARD_COMPATIBLE__ 
#include <util/delay.h>

static volatile uint8_t *_port;
static volatile uint8_t *_ddr;
static uint8_t _pin;

static uint8_t half_clock;

void manchester_init_tx(volatile uint8_t *port, uint8_t pin, uint16_t baud){
	_port = port;
	_pin = pin;
	_ddr = port - 0x1;
	*_ddr |= _BV(pin);	//Enable output on selected pin
	
	half_clock = 1000 / baud;
}

void write_bit(uint8_t bit){
	if (bit) *_port &= ~_BV(_pin);
	else *_port |= _BV(_pin);
	
	_delay_ms(half_clock);
	
	if (bit) *_port |= _BV(_pin);
	else *_port &= ~_BV(_pin);

	_delay_ms(half_clock);
}

void write_byte(uint8_t data) {
	for (uint8_t bit_count = 0; bit_count < 8; bit_count++){
		uint8_t bit = (data & _BV(bit_count));
		write_bit(bit);
	}
}

void manchester_write(uint8_t data) {
	*_port |= _BV(_pin);
	write_byte(0x7E);
	write_byte(data);
	*_port &= ~_BV(_pin);
}
