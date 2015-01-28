#include <avr/io.h>
#include <util/delay.h>

#include "Hd44780_Direct_4bit.h"

using namespace digitalcave;

Hd44780_Direct_4bit::Hd44780_Direct_4bit(uint8_t function, 
								volatile uint8_t *port_rs, uint8_t pin_rs,
								volatile uint8_t *port_e, uint8_t pin_e,
								volatile uint8_t *port_d4, uint8_t pin_d4,
								volatile uint8_t *port_d5, uint8_t pin_d5,
								volatile uint8_t *port_d6, uint8_t pin_d6,
								volatile uint8_t *port_d7, uint8_t pin_d7) {

	this->port_rs = port_rs;
	this->pin_rs = pin_rs;
	this->port_e = port_e;
	this->pin_e = pin_e;
	this->port_d4 = port_d4;
	this->pin_d4 = pin_d4;
	this->port_d5 = port_d5;
	this->pin_d5 = pin_d5;
	this->port_d6 = port_d6;
	this->pin_d6 = pin_d6;
	this->port_d7 = port_d7;
	this->pin_d7 = pin_d7;
	
	*(this->port_rs - 0x1) |= _BV(this->pin_rs);
	*(this->port_e - 0x1) |= _BV(this->pin_e);
	*(this->port_d4 - 0x1) |= _BV(this->pin_d4);
	*(this->port_d5 - 0x1) |= _BV(this->pin_d5);
	*(this->port_d6 - 0x1) |= _BV(this->pin_d6);
	*(this->port_d7 - 0x1) |= _BV(this->pin_d7);
	
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

Hd44780_Direct_4bit::Hd44780_Direct_4bit(uint8_t function) : Hd44780_Direct_4bit::Hd44780_Direct_4bit(function, &PORTB, 0, &PORTB, 1, &PORTB, 2, &PORTB, 3, &PORTB, 4, &PORTB, 5){}

/*
 * Writes the nibble.  Only the bottom 4 bits of b are used.
 */
void Hd44780_Direct_4bit::write_nibble(uint8_t b, uint8_t mode){
	if (mode) *this->port_rs |= _BV(this->pin_rs);	//Set RS high for data
	else *this->port_rs &= ~_BV(this->pin_rs);	//Set RS low for command
	
	//Set data lines low
	*this->port_d4 &= ~_BV(this->pin_d4);
	*this->port_d5 &= ~_BV(this->pin_d5);
	*this->port_d6 &= ~_BV(this->pin_d6);
	*this->port_d7 &= ~_BV(this->pin_d7);
	if (b & 0x01) *this->port_d4 |= _BV(this->pin_d4);
	if (b & 0x02) *this->port_d5 |= _BV(this->pin_d5);
	if (b & 0x04) *this->port_d6 |= _BV(this->pin_d6);
	if (b & 0x08) *this->port_d7 |= _BV(this->pin_d7);

	//Toggle E
	_delay_us(64);
	*this->port_e |= _BV(this->pin_e);	//Set E high to clock in data
	_delay_us(450);
	*this->port_e &= ~_BV(this->pin_e);	//Re-set E back low
	_delay_us(40);
	

}

/*
 * Writes the entire byte, MSB nibble first, then LSB nibble
 */
void Hd44780_Direct_4bit::write_byte(uint8_t b, uint8_t mode){
	//Write the MSB nibble of the byte first...
	write_nibble(((b & 0xF0) >> 4), mode);
	//... then LSB nibble
	write_nibble((b & 0x0F), mode);
}

void Hd44780_Direct_4bit::setByte(uint8_t b) {
	write_byte(b, 1);
}
void Hd44780_Direct_4bit::cmd(uint8_t b) {
	write_byte(b, 0);
}
