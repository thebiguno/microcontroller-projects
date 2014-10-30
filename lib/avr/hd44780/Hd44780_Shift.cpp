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

#include "Hd44780_Shift.h"

using namespace digitalcave;

Hd44780_Shift::Hd44780_Shift(volatile uint8_t *e_port, uint8_t e_pin, volatile uint8_t *rs_port, uint8_t rs_pin, volatile uint8_t *spi_port, uint8_t mosi_pin, uint8_t sclk_pin, uint8_t function) {
	this->e_port = e_port;
	this->e_bv = _BV(e_pin);
	this->rs_port = rs_port;
	this->rs_bv = _BV(rs_pin);
	this->spi_port = spi_port;
	this->mosi_bv = _BV(mosi_pin);
	this->sclk_bv = _BV(sclk_pin);

	*(this->e_port - 0x01) |= this->e_bv;
	*(this->rs_port - 0x01) |= this->rs_bv;
	*(this->spi_port - 0x01) |= (this->mosi_bv | this->sclk_bv);
	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
	
	this->init_display(function);
}

void Hd44780_Shift::setByte(uint8_t b) {
	*this->rs_port |= this->rs_bv;
	SPDR = b;
	while(!(SPSR & (1<<SPIF)));
	_delay_us(64);
	latch();
}
void Hd44780_Shift::cmd(uint8_t b) {
	*this->rs_port &= ~this->rs_bv;
	SPDR = b;
	while(!(SPSR & (1<<SPIF)));
	_delay_us(64);
	latch();
}
