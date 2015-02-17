/*
 * Driver common code for HD44780 in 8 bit mode.  You must also use one of the implementations
 * (Latch, bit banging, etc) to provide the full driver.
 */

#include <avr/io.h>
#include <util/delay.h>
#include "Hd44780.h"

using namespace digitalcave;

void Hd44780::init_display(uint8_t function) {
	this->write_command((function & 0x0c) | 0x30);
	this->write_command(0x08);
	this->clear();
	this->write_command(0x06);
	this->write_command(0x0c);
	
	
}

void Hd44780::clear() {
	this->write_command(0x01);
	_delay_ms(0x04);
}

void Hd44780::home() {
	this->write_command(0x02);
}

void Hd44780::set_mode(uint8_t b) {
	this->write_command((b & 0x03) | 0x04);
}

void Hd44780::set_display(uint8_t b) {
	this->write_command((b & 0x07) | 0x08);
}

void Hd44780::set_shift(uint8_t b) {
	this->write_command((b & 0x0f) | 0x10);
}

void Hd44780::set_cgram_address(uint8_t b) {
	this->write_command((b & 0x3f) | 0x40);
}

void Hd44780::set_ddram_address(uint8_t b) {
	this->write_command((b & 0x7f) | 0x80);
}

void Hd44780::write_bytes(uint8_t bytes[], uint8_t length) {
	for (uint8_t i = 0; i < length; i++) {
		this->write_byte(bytes[i]);
	}
}

void Hd44780::write_bytes(char* bytes, uint8_t length) {
	this->write_bytes((uint8_t*) bytes, length);
}
