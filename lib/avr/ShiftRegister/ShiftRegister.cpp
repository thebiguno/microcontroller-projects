/*
 * An OO implementation of ShiftRegister library.
 */

#include "ShiftRegister.h"
#include <stdlib.h>
#include <avr/interrupt.h>

using namespace digitalcave;

// non-class variables for the ISR
static volatile uint8_t _i;
static volatile uint8_t *_bytes;
static uint8_t _size;
static volatile uint8_t *_latch_port;
static uint8_t _latch_bv;
static void (*_callback)();
static uint8_t _cts = 1;

ShiftRegister::ShiftRegister(volatile uint8_t *spi_port, uint8_t mosi_pin, uint8_t sclk_pin) {
	this->spi_port = spi_port;
	this->mosi_bv = _BV(mosi_pin);
	this->sclk_bv = _BV(sclk_pin);
}

void ShiftRegister::initSpi() {
	*(this->spi_port - 0x1) |= (this->mosi_bv | this->sclk_bv);
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPIE);
}

void ShiftRegister::setCallback(void (*callback)()) {
	_callback = callback;
}

void ShiftRegister::initLatch(volatile uint8_t *port, uint8_t pin) {
	_latch_port = port;
	_latch_bv = _BV(pin);
	
	if (_latch_port != 0) {
		*(port - 0x1) |= _latch_bv;
		*port |= _latch_bv; // set HIGH
	}
}
	
void ShiftRegister::initEnable(volatile uint8_t *port, uint8_t pin) {
	this->enable_port = port;
	this->enable_bv = _BV(pin);
	
	if (this->enable_port != 0) {
		*(port - 0x1) |= _BV(pin);
		*port &= ~_BV(pin); // set LOW (enabled)
	}
}
	
void ShiftRegister::initClear(volatile uint8_t *port, uint8_t pin) {
	this->clear_port = port;
	this->clear_bv = _BV(pin);
	
	if (this->clear_port != 0) {
		*(port - 0x1) |= _BV(pin);
		*port |= _BV(pin); // set HIGH (not clear)
	}	
}

void ShiftRegister::shift(uint8_t b[], uint8_t sz) {
	if (_cts) {
		_size = sz;
		_bytes = b;
		_i = 1;
		SPDR = b[0];
	}
}

uint8_t ShiftRegister::cts() {
	return _cts;
}

void ShiftRegister::clear() {
	*this->clear_port &= ~this->clear_bv;
	*this->clear_port |= this->clear_bv;
}

void ShiftRegister::enable() {
	*this->enable_port &= ~this->enable_bv;
}

void ShiftRegister::disable() {
	*this->enable_port |= this->enable_bv;
}

/////////// interrupts ///////////////

ISR(SPI_STC_vect) {
	if (_i < _size) {
		// copy next byte to the SPDR register
		SPDR = _bytes[_i++];
	} else if (_i == _size) {
		if (_latch_port != 0) {
			*_latch_port &= ~_latch_bv;
			*_latch_port |= _latch_bv;
		}
		_cts = 1;
		if (_callback != 0) {
			((*_callback)());
		}
	}
} 


