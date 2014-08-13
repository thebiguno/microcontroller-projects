/*
 * Driver for HD44780 in 8 bit mode using SPI and a shift register with no latch.
 * This driver requires MOSI and SCK as well as one other pin connected to E.  MOSI drives both the shift register and RS.
 */

#include <avr/io.h>
#include <util/delay_basic.h>

void hd44780_init() {
	
	DDRB |= (1<<PB3) | (1<<PB5); // Set MOSI and SCK output
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0); // enable, master, F_CPU/64
	
	hd44780_clear();
}

/* 
 * Clear all display data, and set DDRAM address to 0x00.
 * Return cursor to the left edge of the first line.
 * Make the entry mode 'increment' (I/D = 1).
 */
void hd44780_clear() {
	cmd(0x01);
}

/* 
 * Set DDRAM address to 0x00.
 * Return cursor to the left edge of the first line.
 * Return the display to its original status if shifted.
 * Contents of DDRAM are NOT changed.
 */
void hd44780_home() {
	cmd(0x02);
}

/* 
 * Set the moving direction of cursor and display.
 * Bit 0 (SH) is the display shift control; 0 = no shift, 1 = shift.
 * Bit 1 (I/D) is the increment/decrement control; 0 = left/decrement, 1 = right/increment.
 */
void hd44780_set_mode(uint8_t b) {
	cmd(b & 0x03 | 0x04);
}

/* 
 * Set the moving direction of cursor and display.
 * Bit 0 (B) is cursor blink on/off control; 0 = off, 1 = on.
 * Bit 1 (C) is cursor on/off control; 0 = off, 1 = on.
 * Bit 2 (D) is display on/off control; 0 = off, 1 = on.
 */
void hd44780_set_display(uint8_t b) {
	cmd(b & 0x07 | 0x08);
}

/* 
 * Shifts the cursor or screen the left or right.
 * Bit 2 (R/L) is left/right control; 0 = left, 1 = right.
 * Bit 3 (S/C) is screen/cursor control; 0 = cursor, 1 = screen.
 */
void hd44780_set_shift(uint8_t b) {
	cmd(b & 0x0f | 0x10);
}

/* 
 * Set the moving direction of cursor and display.
 * Bit 2 (F) is dot format control; 0 = 5x8, 1 = 5x11.
 * Bit 3 (N) is line number control; 0 = 1 line, 1 = 2 line.
 * Bit 4 (DL) is data length control; 0 = 4-bit, 1 = 8-bit.
 */
void hd44780_set_function(uint8_t b) {
	cmd(b & 0x1f | 0x10 | 0x20);
}

/*
 * Sets the address pointer for CGRAM, allowing read or write of a byte of CGRAM data.
 */
void hd44780_set_cgram_address(uint8_t b) {
	cmd(b & 0x3f | 0x40);
}

/*
 * Sets the address pointer for DDRAM, allowing read or write of a byte of DDRAM data.
 * For a 1 line display, the valid address range is 0x00 to 0x4f.
 * For a 2 line display, the valid address range is 0x00 to 0x27 for line one and 0x40 to 0x67 for line 2.
 * (displays with more lines have individually addressable modules).
 */
void hd44780_set_ddram_address(uint8_t address) {
	cmd(b & 0x7f | 0x80);
}

/*
 * Writes a byte of data to either DDRAM or CGRAM.
 * The address may be automatically incremented/decremented according to the entry mode.
 */
void hd44780_set(uint8_t b) {
	rs1();
	SPDR = b;
	while(!(SPSR & (1<<SPIF)));
	latch();
}

/*
 * Reads the busy flag and DDRAM address.
 * Bit 7 (BF) is the busy flag; 0 = not busy, 1 = busy.
 * Bits 0 to 7 (AC) are the DDRAM address.
 */
uint8_t hd44780_get_busyflag_address() {
	// this driver can't implement this since the data pins are connected to the shift register
	return 0;
}

/*
 * Reads a byte of data to either DDRAM or CGRAM.
 * The address may be automatically incremented/decremented according to the entry mode.
 */
uint8_t hd44780_get() {
	// this driver can't implement this since the data pins are connected to the shift register
	return 0;
}

/*
 * Writes sz bytes to either CGRAM or DDRAM.  The starting address must be set in advance and no bounds checking is done.
 */
void hd44780_set_c(char* text, uint8_t sz) {
	for (uint8_t i = 0; i < sz; i++) {
		hd44780_set(text[i]);
	}
}
/*
 * Writes sz bytes to either CGRAM or DDRAM.  The starting address must be set in advance and no bounds checking is done.
 */
void hd44780_set_b(uint8_t* bytes, uint8_t sz) {
	for (uint8_t i = 0; i < sz; i++) {
		hd44780_set(bytes[i]);
	}
}

inline void rs0() {
	PORTB |= ~_BV(PB3); // set MOSI/RS low
}
inline void rs1() {
	PORTB &= _BV(PB3); // set MOSI/RS high
}
inline void cmd(uint8_t b) {
	rs0();
	SPDR = b;
	while(!(SPSR & (1<<SPIF)));
	latch();
}
inline void latch() {
	#ifdef HD44780_LATCH
	// pulse SCK one more time latch the current data on the shift register
	// this is required when using a shift register with a latch and CLK and RCLK are wired together
	PORTB |= _BV(PB5);
	PORTB &= ~_BV(PB5);
	#endif

	HD44780_E_PORT &= _BV(HD44780_E_PIN);
	pulse();
	HD44780_E_PORT |= ~_BV(HD44780_E_PIN);
	pulse();
	#endif
}	

inline void pulse() {
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
