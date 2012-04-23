/*
 * An OO implementation of shift library.
 */

#include "shift.h"
#include <stdlib.h>
#include <avr/interrupt.h>


// TODO make this conditional on the chip
#define DD_MOSI DDB3
#define DD_SCK DDB5
#define DDR_SPI DDRB

// non-class variables for the ISR
static volatile uint8_t _i;
static uint8_t _size;
static volatile uint8_t *_buf;
static volatile uint8_t *_latch_port;
static uint8_t _latch_pin;
static void (*_callback)();


Shift::Shift(uint8_t size) {
	_size = size - 1;
	_buf = (uint8_t*) malloc(_size);
	
	DDR_SPI |= _BV(DD_SCK);
	DDR_SPI |= _BV(DD_MOSI);
	// DDR_SPI |= _BV(DDB4);
	
	// setup SPI (enable, master, interrupts) (clk/128)
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPIE) | _BV(SPR0) | _BV(SPR1);

	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif	
}

void Shift::setCallback(void (*callback)()) {
	_callback = callback;
}

void Shift::setLatch(volatile uint8_t *port, uint8_t pin) {
	_latch_port = port;
	_latch_pin = pin;
	
	if (_latch_port != 0) {
		*(port - 0x1) |= _BV(pin);
		*port |= _BV(pin); // set HIGH
	}
}
	
void Shift::setEnable(volatile uint8_t *port, uint8_t pin) {
	this->enable_port = port;
	this->enable_pin = pin;
	
	if (this->enable_port != 0) {
		*(port - 0x1) |= _BV(pin);
		*port &= ~_BV(pin); // set LOW (enabled)
	}
}
	
void Shift::setClear(volatile uint8_t *port, uint8_t pin) {
	this->clear_port = port;
	this->clear_pin = pin;
	
	if (this->clear_port != 0) {
		*(port - 0x1) |= _BV(pin);
		*port |= _BV(pin); // set HIGH (not clear)
	}	
}

void Shift::shift(uint8_t b[]) {
	SPCR |= _BV(MSTR); // this shouldn't be required but something keeps clearing MSTR
	// if(!(SPSR & (1<<SPIF))) return;

	for (int i = 0; i < _size; i++) {
		_buf[i] = b[i+1];
	}
	
	_i = 0;
	SPDR = b[0];
}

void Shift::clear() {
	*this->clear_port &= ~_BV(this->clear_pin);
	*this->clear_port |= _BV(this->clear_pin);	
}

void Shift::enable() {
	*this->enable_port &= ~_BV(this->enable_pin);
}

void Shift::disable() {
	*this->enable_port |= _BV(this->enable_pin);
}

/////////// interrupts ///////////////

ISR(SPI_STC_vect) {
	if (_i < _size) {
		SPDR = _buf[_i++];
	} else if (_i == _size) {
		if (_latch_port != 0) {
			*_latch_port &= ~_BV(_latch_pin);
			*_latch_port |= _BV(_latch_pin);
		}
		if (_callback != 0) {
			((*_callback)());
		}
	}
} 


