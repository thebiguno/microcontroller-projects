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
#include "Hd44780.h"

using namespace digitalcave;

Hd44780::Hd44780(volatile uint8_t *e_port, uint8_t e_pin, volatile uint8_t *spi_port, uint8_t mosi_pin, uint8_t sclk_pin, uint8_t function) {
	this->e_port = e_port;
	this->e_bv = _BV(e_pin);
	this->spi_port = spi_port;
	this->mosi_bv = _BV(mosi_pin);
	this->sclk_bv = _BV(sclk_pin);

	*(this->e_port - 0x01) |= this->e_bv;
	*(this->spi_port - 0x01) |= (this->mosi_bv | this->sclk_bv);
//	SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0) | _BV(SPR1);

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

void rs0();
void rs1();
void latch();
void data(uint8_t);

void Hd44780::setByte(uint8_t b) {
//	SPDR = b;
//	while(!(SPSR & (1<<SPIF)));
	this->data(b);
	*this->spi_port |= this->mosi_bv;
	latch();
}
void Hd44780::cmd(uint8_t b) {
//	SPDR = b;
//	while(!(SPSR & (1<<SPIF)));
	this->data(b);
	*this->spi_port &= ~this->mosi_bv;
	latch();
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
void Hd44780::latch() {
	*this->e_port |= this->e_bv;
	_delay_us(450);
	*this->e_port &= ~this->e_bv;
	_delay_us(40);
}

void Hd44780::data(uint8_t out_byte) {
	/* make sure clock is low */
	*this->spi_port &= ~this->sclk_bv;
        
        int i;
        for(i=0; i<8; i++)
        {
                /* loop through bits */
                
                if (out_byte & 0x80)
                {
                        /* this bit is high */
					*this->spi_port |= this->mosi_bv;
                }
                else
                {
                        /* this bit is low */
					*this->spi_port &= ~this->mosi_bv;
                }
                out_byte = out_byte << 1;
                
	/* pulse the the shift register clock */
	*this->spi_port |= this->sclk_bv;
	_delay_ms(1);
	*this->spi_port &= ~this->sclk_bv;
	}
}

