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
#include <util/delay_basic.h>

using namespace digitalcave;

Hd44780::Hd44780(volatile uint8_t *e_port, uint8_t e_pin, volatile uint8_t *spi_port, uint8_t mosi_pin, uint8_t sclk_pin) {
	this->e_port = e_port;
	this->e_bv = _BV(e_pin);
	this->spi_port = spi_port;
	this->mosi_bv = _BV(mosi_pin);
	this->sclk_bv = _BV(sclk_pin);
}

void Hd44780::clear() {
	this->cmd(0x01);
}

void Hd44780::home() {
	this->cmd(0x02);
}

void Hd44780::setMode(uint8_t b) {
	this->cmd(b & 0x03 | 0x04);
}

void Hd44780::setDisplay(uint8_t b) {
	this->cmd(b & 0x07 | 0x08);
}

void Hd44780::setShift(uint8_t b) {
	this->cmd(b & 0x0f | 0x10);
}

void Hd44780::setFunction(uint8_t b) {
	this->cmd(b & 0x1f | 0x10 | 0x20);
}

void Hd44780::setCgramAddress(uint8_t b) {
	this->cmd(b & 0x3f | 0x40);
}

void Hd44780::setDdramAddress(uint8_t address) {
	this->cmd(b & 0x7f | 0x80);
}

void rs0();
void rs1();
void latch();

void Hd44780::setByte(uint8_t b) {
	SPDR = b;
	while(!(SPSR & (1<<SPIF)));
	rs1();
	latch();
}
void Hd44780::cmd(uint8_t b) {
	SPDR = b;
	while(!(SPSR & (1<<SPIF)));
	rs0();
	latch();
}

void Hd44780::setText(char* text, uint8_t sz) {
	for (uint8_t i = 0; i < sz; i++) {
		this->set(text[i]);
		_delay_us(37);
	}
}
void Hd44780::setBytes(uint8_t* bytes, uint8_t sz) {
	for (uint8_t i = 0; i < sz; i++) {
		this->set(bytes[i]);
		_delay_us(37);
	}
}

inline void rs0() {
	this->spi_port &= ~this->mosi_bv;
}
inline void rs1() {
	this->spi_port |= this->mosi_bv;
}
inline void wait() {
	#if F_CPU > 3333333
	asm volatile("nop\n\t" 
	#if F_CPU > 6666666
	"nop\n\t" 
	#endif
	#if F_CPU > 9999999
	"nop\n\t" 
	#endif
	#if F_CPU > 13333333
	"nop\n\t" 
	#endif
	#if F_CPU > 16666666
	"nop\n\t" 
	#endif
	#if F_CPU > 19999999
	"nop\n\t" 
	#endif
	#if F_CPU > 23333333
	"nop\n\t" 
	#endif
	::);
	#endif
}
inline void latch() {
	#ifdef HD44780_LATCH
	// pulse SCK one more time latch the current data on the shift register
	// this is required when using a shift register with a latch and CLK and RCLK are wired together
	this->spi_port |= this->sclk_bv;
	this->spi_port &= ~this->sclk_bv;
	#endif

	this->e_port |= this->e_bv;
	wait();
	this->e_port &= ~this->e_bv;
	wait();
	#endif
}

