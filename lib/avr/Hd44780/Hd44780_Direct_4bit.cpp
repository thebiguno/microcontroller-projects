/*
 * Driver for HD44780 in 4 bit mode using direct pin access.
 * Current hardcoded to use PORTB2..5 for data pins, PORTB0 for RS, and 
 * PORTB1 for E.
 */

#include <avr/io.h>
#include <util/delay.h>

#include "Hd44780_Direct.h"

using namespace digitalcave;

Hd44780_Direct::Hd44780_Direct(uint8_t function) {
	DDRD = 0xFF;
	DDRB |= _BV(PORTB0) | _BV(PORTB1) | _BV(PORTB2) | _BV(PORTB3) | _BV(PORTB4) | _BV(PORTB5);
	
	
	
	this->cmd((function & 0x0c) | 0x20);	//4 bit, plus function (char size + lines)
	_delay_us(0x3f);
	this->cmd((function & 0x0c) | 0x20);	//The second time (need this for 4 bit mode)
	_delay_us(0x3f);

	this->cmd(0x0C);	//Display on, cursor off, cursor blink off
	this->clear();		//Clear display
}

static void write(uint8_t b, uint8_t mode){
	if (mode)
		PORTB |= _BV(PORTB0);	//Set RS high for data
	else 
		PORTB &= ~_BV(PORTB0);	//Set RS low for command

	PORTB &= 0xC3;		//Set bits 2..5 low
	PORTB |= (b & 0x0F) << 2;
	
	_delay_us(64);
	PORTB |= _BV(PORTB2);	//Set E high to clock in data
	_delay_us(450);
	PORTB &= ~_BV(PORTB2);	//Re-set E back low
	_delay_us(40);
	
	PORTB &= 0xC3;		//Set bits 2..5 low
	PORTB |= (b & 0xF0) >> 2;
	
	_delay_us(64);
	PORTB |= _BV(PORTB2);	//Set E high to clock in data
	_delay_us(450);
	PORTB &= ~_BV(PORTB2);	//Re-set E back low
	_delay_us(40);

}

void Hd44780_Direct::setByte(uint8_t b) {
	write(b, 1);
}
void Hd44780_Direct::cmd(uint8_t b) {
	write(b, 0);
}
