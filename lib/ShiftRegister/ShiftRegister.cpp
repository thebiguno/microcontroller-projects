/*
 * An OO implementation of ShiftRegister library.
 */

#include "ShiftRegister.h"
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>


// TODO make this conditional on the chip
#define DD_SS DDB2
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
static uint8_t _cts = 1;

ShiftRegister::ShiftRegister(uint8_t size) {
	_size = size - 1;
	_buf = (uint8_t*) malloc(_size);
	
	// configure MOSI, SCK, SS as output pins
	DDR_SPI |= _BV(DD_SCK) | _BV(DD_MOSI) | _BV(DD_SS);
	
	// setup SPI (enable, master, interrupts)
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPIE);

	// clear SPI interrupt flag by reading SPSR and SPDR
	// uint8_t x = SPSR;
	// x = SPDR;
	
	// set SS high
	PORTB |= _BV(DD_SS);

	//Enable global interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif	
}

void ShiftRegister::setCallback(void (*callback)()) {
	_callback = callback;
}

void ShiftRegister::setLatch(volatile uint8_t *port, uint8_t pin) {
	_latch_port = port;
	_latch_pin = pin;
	
	if (_latch_port != 0) {
		*(port - 0x1) |= _BV(pin);
		*port |= _BV(pin); // set HIGH
	}
}
	
void ShiftRegister::setEnable(volatile uint8_t *port, uint8_t pin) {
	this->enable_port = port;
	this->enable_pin = pin;
	
	if (this->enable_port != 0) {
		*(port - 0x1) |= _BV(pin);
		*port &= ~_BV(pin); // set LOW (enabled)
	}
}
	
void ShiftRegister::setClear(volatile uint8_t *port, uint8_t pin) {
	this->clear_port = port;
	this->clear_pin = pin;
	
	if (this->clear_port != 0) {
		*(port - 0x1) |= _BV(pin);
		*port |= _BV(pin); // set HIGH (not clear)
	}	
}

void ShiftRegister::shift(uint8_t b[]) {
	if (_cts) {
		//SPCR |= _BV(MSTR); // re-enable master mode; disbled when SS is set low
		_cts = 0;
		
		for (int i = 0; i < _size; i++) {
			_buf[i] = b[i+1];
		}
	
		_i = 0;
		SPDR = b[0];
	}
}

uint8_t ShiftRegister::cts() {
	return _cts;
}

void ShiftRegister::clear() {
	*this->clear_port &= ~_BV(this->clear_pin);
	*this->clear_port |= _BV(this->clear_pin);	
}

void ShiftRegister::enable() {
	*this->enable_port &= ~_BV(this->enable_pin);
}

void ShiftRegister::disable() {
	*this->enable_port |= _BV(this->enable_pin);
}

/////////// interrupts ///////////////

ISR(SPI_STC_vect) {
	if (_i < _size) {
		// copy next byte to the SPDR register
		SPDR = _buf[_i++];
	} else if (_i == _size) {
		if (_latch_port != 0) {
			*_latch_port &= ~_BV(_latch_pin);
			// _delay_us(5);
			*_latch_port |= _BV(_latch_pin);
		}
		_cts = 1;
		if (_callback != 0) {
			((*_callback)());
		}
	}
} 


