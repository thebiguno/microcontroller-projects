/*
 * Driver common code for HD44780 in 8 bit mode.  You must also use one of the implementations
 * (Latch, bit banging, etc) to provide the full driver.
 */

#include <avr/io.h>
#include <util/delay.h>
#include "Hd44780.h"

using namespace digitalcave;

void Hd44780::init_display(uint8_t function) {
	// _delay_ms(0x7f);
	// this->cmd(0x30);	// function set: 8-bit interface
	// _delay_ms(0x08);
	// this->cmd(0x30);	// function set: 8-bit interface
	// _delay_us(0x7f);
	// this->cmd(0x30);	// function set: 8-bit interface
	// _delay_us(0x7f);
	this->cmd((function & 0x0c) | 0x30);
	_delay_us(0x3f);
	this->cmd(0x08);
	_delay_us(0x3f);
	this->clear();
	_delay_ms(0x04);
	this->cmd(0x06);
	_delay_us(0x3f);
	this->cmd(0x0c);
	_delay_us(0x3f);
	
	
}

void Hd44780::clear() {
	this->cmd(0x01);
}

void Hd44780::home() {
	this->cmd(0x02);
}

void Hd44780::setMode(uint8_t b) {
	this->cmd((b & 0x03) | 0x04);
}

void Hd44780::setDisplay(uint8_t b) {
	this->cmd((b & 0x07) | 0x08);
}

void Hd44780::setShift(uint8_t b) {
	this->cmd((b & 0x0f) | 0x10);
}

void Hd44780::setCgramAddress(uint8_t b) {
	this->cmd((b & 0x3f) | 0x40);
}

void Hd44780::setDdramAddress(uint8_t b) {
	this->cmd((b & 0x7f) | 0x80);
}

void Hd44780::setText(char* text, uint8_t sz) {
	for (uint8_t i = 0; i < sz; i++) {
		this->setByte(text[i]);
		_delay_us(64);
	}
}
void Hd44780::setBytes(uint8_t bytes[], uint8_t sz) {
	for (uint8_t i = 0; i < sz; i++) {
		this->setByte(bytes[i]);
		_delay_us(64);
	}
}

