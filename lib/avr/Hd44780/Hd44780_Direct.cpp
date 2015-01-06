/*
 * Driver for HD44780 in 8 bit mode using SPI and a shift register with or without a latch.
 * If the shift register has a latch it must be connected with SCK and HD44780_LATCH must be defined.
 * This driver uses the SPI hardware and requires MOSI and SCK as well as one other pin connected to E.
 * MOSI and SCK may be shared with other SPI devices.  MOSI drives both the shift register and RS.  E functions as type of chip select.
 * Q1~Q8 on the shift register connect to D1~D8 on the HD44780.
 * MOSI on the AVR connects to DI on the shift register and RS on the HD44780.
 * SCK on the AVR connects to CLK (and RCLK) on the shift register.
 */

#include <avr/io.h>
#include <util/delay.h>

#include "Hd44780_Direct.h"

using namespace digitalcave;

Hd44780_Direct::Hd44780_Direct(uint8_t function) {
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
