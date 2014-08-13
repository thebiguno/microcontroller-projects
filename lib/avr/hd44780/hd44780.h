#include <avr/io.h>
#include <util/delay.h>

using namespace digitalcave;

class Hd44780 {
	
	private:
	volatile uint8_t *e_port;
	volatile uint8_t e_bv;
	volatile uint8_t *spi_port;
	volatile uint8_t mosi_bv;
	volatile uint8_t sclk_bv;
	void cmd(uint8_t cmd);

	public:
	/*
	 * Creates a new HD44780 module in 3-wire mode.
	 * The port and pin are that are connected to E on the module.
	 * MOSI and SCK must be configured for output externally.
	 * SPI must be configured as master and enabled externally.
	 * MOSI must be connected to DI on the shift register and to RS on the HD44780 module.
	 * SCK must be connected to SCK on the shift register.
	 */
	Hd44780(volatile uint8_t *port, uint8_t pin);
	
	// TODO add constructors for 2-wire and 7-wire operation
	
	/* 
	 * Clear all display data, and set DDRAM address to 0x00.
	 * Return cursor to the left edge of the first line.
	 * Make the entry mode 'increment' (I/D = 1).
	 * The device will remain busy for 1.52 ms.
	 */
	void clear();

	/* 
	 * Set DDRAM address to 0x00.
	 * Return cursor to the left edge of the first line.
	 * Return the display to its original status if shifted.
	 * Contents of DDRAM are NOT changed.
	 * The device will remain busy for 1.52 ms.
	 */
	void home();

	/* 
	 * Set the moving direction of cursor and display.
	 * Bit 0 (SH) is the display shift control; 0 = no shift, 1 = shift.
	 * Bit 1 (I/D) is the increment/decrement control; 0 = left/decrement, 1 = right/increment.
	 * The device will remain busy for 37 us.
	 */
	void setMode(uint8_t mode);

	/* 
	 * Set the moving direction of cursor and display.
	 * Bit 0 (B) is cursor blink on/off control; 0 = off, 1 = on.
	 * Bit 1 (C) is cursor on/off control; 0 = off, 1 = on.
	 * Bit 2 (D) is display on/off control; 0 = off, 1 = on.
	 * The device will remain busy for 37 us.
	 */
	void setDisplay(uint8_t display);

	/* 
	 * Shifts the cursor or screen the left or right.
	 * Bit 2 (R/L) is left/right control; 0 = left, 1 = right.
	 * Bit 3 (S/C) is screen/cursor control; 0 = cursor, 1 = screen.
	 * The device will remain busy for 37 us.
	 */
	void setShift(uint8_t shift);

	/* 
	 * Set the moving direction of cursor and display.
	 * Bit 2 (F) is dot format control; 0 = 5x8, 1 = 5x11.
	 * Bit 3 (N) is line number control; 0 = 1 line, 1 = 2 line.
	 * Bit 4 (DL) is data length control; 0 = 4-bit, 1 = 8-bit.
	 * The device will remain busy for 37 us.
	 */
	void setFunction(uint8_t function);

	/*
	 * Sets the address pointer for CGRAM, allowing read or write of a byte of CGRAM data.
	 * Depending on the module, CGRAM is either 0x00 to 0x07 or 0x00 to 0x0f.
	 * The device will remain busy for 37 us.
	 */
	void setCgramAddress(uint8_t address);

	/*
	 * Sets the address pointer for DDRAM, allowing read or write of a byte of DDRAM data.
	 * The device will remain busy for 37 us.
	 */
	void setDdramAddress(uint8_t address);

	/*
	 * Writes a byte of data to either DDRAM or CGRAM.
	 * The address may be automatically incremented/decremented according to the entry mode.
	 * The device will remain busy for 37 us.
	 */
	void setByte(uint8_t b);

	/*
	 * Reads the busy flag and DDRAM address.
	 * Bit 7 (BF) is the busy flag; 0 = not busy, 1 = busy.
	 * Bits 0 to 7 (AC) are the DDRAM address.
	 */
	uint8_t getBusyFlagAddress();

	/*
	 * Reads a byte of data to either DDRAM or CGRAM.
	 * The address may be automatically incremented/decremented according to the entry mode.
	 */
	uint8_t getByte();

	/*
	 * Writes sz bytes to either CGRAM or DDRAM.  The starting address must be set in advance and no bounds checking is done.
	 */
	void setText(char* text, uint8_t sz);

	/*
	 * Writes sz bytes to either CGRAM or DDRAM.  The starting address must be set in advance and no bounds checking is done.
	 */
	void setBytes(uint8_t* bytes, uint8_t sz);
}