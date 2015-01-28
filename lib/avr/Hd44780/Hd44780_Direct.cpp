/*
 * Driver for HD44780 in 8 bit mode using direct pin access.
 * Current hardcoded to use PORTD for data pins, PORTB1 for RS, and 
 * PORTB2 for E.
 */

#include <avr/io.h>
#include <util/delay.h>

#include "Hd44780_Direct.h"

using namespace digitalcave;

Hd44780_Direct::Hd44780_Direct(uint8_t function) {
	//TODO do proper setup procedure
	DDRD = 0xFF;
	DDRB |= _BV(PORTB1) | _BV(PORTB2);
	
	this->init_display(function);
}

void Hd44780_Direct::setByte(uint8_t b) {
	PORTB |= _BV(PORTB1);	//Set RS high for data
	PORTD = b;
	_delay_us(64);
	PORTB |= _BV(PORTB2);
	_delay_us(450);
	PORTB &= ~_BV(PORTB2);
	_delay_us(40);
}
void Hd44780_Direct::cmd(uint8_t b) {
	PORTB &= ~_BV(PORTB1);	//Set RS low for command
	PORTD = b;
	_delay_us(64);
	PORTB |= _BV(PORTB2);
	_delay_us(450);
	PORTB &= ~_BV(PORTB2);
	_delay_us(40);
}
