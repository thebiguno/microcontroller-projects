/*
 * Driver for HD44780 in 4 bit mode using direct pin access.
 * Current hardcoded to use PORTB2..5 for data pins, PORTB0 for RS, and 
 * PORTB1 for E.
 */

#include <avr/io.h>
#include <util/delay.h>

#include "Hd44780_Direct.h"

using namespace digitalcave;

static void write_nibble(uint8_t b, uint8_t mode);

Hd44780_Direct::Hd44780_Direct(uint8_t function) {
	DDRD = 0xFF;
	DDRB |= _BV(PORTB0) | _BV(PORTB1) | _BV(PORTB2) | _BV(PORTB3) | _BV(PORTB4) | _BV(PORTB5);
	
	_delay_ms(100);
	write_nibble(0x03, 0);
	_delay_ms(10);
	write_nibble(0x03, 0);
	_delay_ms(1);
	write_nibble(0x03, 0);
	_delay_ms(1);
	write_nibble(0x02, 0);
	_delay_ms(1);
	
	this->cmd((function & 0x0c) | 0x20);	//Function set (char size + lines)
	_delay_us(60);

	this->cmd(0x08);	//Display off
	_delay_us(60);
	this->cmd(0x01);	//Clear display
	_delay_ms(5);
	this->cmd(0x04);	//Entry mode set
	_delay_ms(60);
	this->cmd(0x0C);	//Display on, cursor off, cursor blink off
	this->clear();		//Clear display
}

/*
 * Writes the nibble.  Only the bottom 4 bits of b are used.
 */
static void write_nibble(uint8_t b, uint8_t mode){
	if (mode)
		PORTB |= _BV(PORTB0);	//Set RS high for data
	else 
		PORTB &= ~_BV(PORTB0);	//Set RS low for command
	
	//Set data lines
	PORTB &= 0xC3;		//Set bits 2..5 low
	PORTB |= ((b & 0x0F) << 2);	//Set bits 2..5 to specified nibble

	//Toggle E
	_delay_us(64);
	PORTB |= _BV(PORTB1);	//Set E high to clock in data
	_delay_us(450);
	PORTB &= ~_BV(PORTB1);	//Re-set E back low
	_delay_us(40);
	

}

/*
 * Writes the entire byte, MSB nibble first, then LSB nibble
 */
static void write_byte(uint8_t b, uint8_t mode){
	//Write the MSB nibble of the byte first...
	write_nibble(((b & 0xF0) >> 4), mode);
	//... then LSB nibble
	write_nibble((b & 0x0F), mode);
}

void Hd44780_Direct::setByte(uint8_t b) {
	write_byte(b, 1);
}
void Hd44780_Direct::cmd(uint8_t b) {
	write_byte(b, 0);
}
