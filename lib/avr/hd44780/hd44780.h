#include <avr/io.h>

void hd44780_init();

/* 
 * Clear all display data, and set DDRAM address to 0x00.
 * Return cursor to the left edge of the first line.
 * Make the entry mode 'increment' (I/D = 1).
 */
void hd44780_clear();

/* 
 * Set DDRAM address to 0x00.
 * Return cursor to the left edge of the first line.
 * Return the display to its original status if shifted.
 * Contents of DDRAM are NOT changed.
 */
void hd44780_home();

/* 
 * Set the moving direction of cursor and display.
 * Bit 0 (SH) is the shift control; 0 = manual, 1 = automatic.
 * Bit 1 (I/D) is the increment/decrement control; 0 = left/decrement, 1 = right/increment.
 */
void hd44780_set_mode(uint8_t mode);

/* 
 * Set the moving direction of cursor and display.
 * Bit 0 (B) is cursor blink on/off control; 0 = off, 1 = on.
 * Bit 1 (C) is cursor on/off control; 0 = off, 1 = on.
 * Bit 2 (D) is display on/off control; 0 = off, 1 = on.
 */
void hd44780_set_display(uint8_t display);

/* 
 * Shifts the cursor or screen the left or right.
 * Bit 2 (R/L) is left/right control; 0 = left, 1 = right.
 * Bit 3 (S/C) is screen/cursor control; 0 = cursor, 1 = screen.
 */
void hd44780_set_shift(uint8_t shift);

/* 
 * Set the moving direction of cursor and display.
 * Bit 2 (F) is dot format control; 0 = 5x8, 1 = 5x11.
 * Bit 3 (N) is line number control; 0 = 1 line, 1 = 2 line.
 * Bit 4 (DL) is data length control; 0 = 4-bit, 1 = 8-bit.
 */
void hd44780_set_function(uint8_t function);

/*
 * Sets the address pointer for CGRAM, allowing read or write of a byte of CGRAM data.
 */
void hd44780_set_cgram_address(uint8_t address);

/*
 * Sets the address pointer for DDRAM, allowing read or write of a byte of DDRAM data.
 */
void hd44780_set_ddram_address(uint8_t address);

/*
 * Reads the busy flag and DDRAM address.
 * Bit 7 (BF) is the busy flag; 0 = not busy, 1 = busy.
 * Bits 0 to 7 (AC) are the DDRAM address.
 */
uint8_t hd44780_get_busyflag_address();

/*
 * Writes a byte of data to either DDRAM or CGRAM.
 * The address may be automatically incremented/decremented according to the entry mode.
 */
void hd44780_set(uint8_t b);

/*
 * Reads a byte of data to either DDRAM or CGRAM.
 * The address may be automatically incremented/decremented according to the entry mode.
 */
uint8_t hd44780_get();

/* 
 * Position the cursor and write text.
 */
void hd44780_write_buffer_bounds(uint8_t x, uint8_t y, char* text, uint8_t sz);