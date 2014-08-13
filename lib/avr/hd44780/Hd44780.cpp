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
/* 
 * Clear all display data, and set DDRAM address to 0x00.
 * Return cursor to the left edge of the first line.
 * Make the entry mode 'increment' (I/D = 1).
 */
void Hd44780::clear() {
	this->cmd(0x01);
}

/* 
 * Set DDRAM address to 0x00.
 * Return cursor to the left edge of the first line.
 * Return the display to its original status if shifted.
 * Contents of DDRAM are NOT changed.
 */
void Hd44780::home() {
	this->cmd(0x02);
}

/* 
 * Set the moving direction of cursor and display.
 * Bit 0 (SH) is the display shift control; 0 = no shift, 1 = shift.
 * Bit 1 (I/D) is the increment/decrement control; 0 = left/decrement, 1 = right/increment.
 */
void Hd44780::setMode(uint8_t b) {
	this->cmd(b & 0x03 | 0x04);
}

/* 
 * Set the moving direction of cursor and display.
 * Bit 0 (B) is cursor blink on/off control; 0 = off, 1 = on.
 * Bit 1 (C) is cursor on/off control; 0 = off, 1 = on.
 * Bit 2 (D) is display on/off control; 0 = off, 1 = on.
 */
void Hd44780::setDisplay(uint8_t b) {
	this->cmd(b & 0x07 | 0x08);
}

/* 
 * Shifts the cursor or screen the left or right.
 * Bit 2 (R/L) is left/right control; 0 = left, 1 = right.
 * Bit 3 (S/C) is screen/cursor control; 0 = cursor, 1 = screen.
 */
void Hd44780::setShift(uint8_t b) {
	this->cmd(b & 0x0f | 0x10);
}

/* 
 * Set the moving direction of cursor and display.
 * Bit 2 (F) is dot format control; 0 = 5x8, 1 = 5x11.
 * Bit 3 (N) is line number control; 0 = 1 line, 1 = 2 line.
 * Bit 4 (DL) is data length control; 0 = 4-bit, 1 = 8-bit.
 */
void Hd44780::setFunction(uint8_t b) {
	this->cmd(b & 0x1f | 0x10 | 0x20);
}

/*
 * Sets the address pointer for CGRAM, allowing read or write of a byte of CGRAM data.
 */
void Hd44780::setCgramAddress(uint8_t b) {
	this->cmd(b & 0x3f | 0x40);
}

/*
 * Sets the address pointer for DDRAM, allowing read or write of a byte of DDRAM data.
 * For a 1 line display, the valid address range is 0x00 to 0x4f.
 * For a 2 line display, the valid address range is 0x00 to 0x27 for line one and 0x40 to 0x67 for line 2.
 * (displays with more lines have individually addressable modules).
 */
void Hd44780::setDdramAddress(uint8_t address) {
	this->cmd(b & 0x7f | 0x80);
}

void rs0();
void rs1();
void latch();

/*
 * Writes a byte of data to either DDRAM or CGRAM.
 * The address may be automatically incremented/decremented according to the entry mode.
 */
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

/*
 * Reads the busy flag and DDRAM address.
 * Bit 7 (BF) is the busy flag; 0 = not busy, 1 = busy.
 * Bits 0 to 7 (AC) are the DDRAM address.
 */
uint8_t Hd44780::getBusyFlagAddress() {
	// can't implement this since the data pins are connected to the shift register
	return 0;
}

/*
 * Reads a byte of data to either DDRAM or CGRAM.
 * The address may be automatically incremented/decremented according to the entry mode.
 */
uint8_t Hd44780::getByte() {
	// can't implement this since the data pins are connected to the shift register
	return 0;
}

/*
 * Writes sz bytes to either CGRAM or DDRAM.  The starting address must be set in advance and no bounds checking is done.
 */
void Hd44780::setText(char* text, uint8_t sz) {
	for (uint8_t i = 0; i < sz; i++) {
		this->set(text[i]);
		_delay_us(37);
	}
}
/*
 * Writes sz bytes to either CGRAM or DDRAM.  The starting address must be set in advance and no bounds checking is done.
 */
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

