#ifndef HD44780_H
#define HD44780_H

#include <avr/io.h>
#include <stdlib.h>

namespace digitalcave {

	class Hd44780 {
	
		private:
		volatile uint8_t *e_port;
		uint8_t e_bv;
		volatile uint8_t *rs_port;
		uint8_t rs_bv;
		volatile uint8_t *spi_port;
		uint8_t mosi_bv;
		uint8_t sclk_bv;
		void cmd(uint8_t cmd);
		void latch();

		public:
			static const uint8_t MODE_SHIFT_OFF = 0x00;
			static const uint8_t MODE_SHIFT_ON = 0x01;
			static const uint8_t MODE_CURSOR_LEFT_DECR = 0x00;
			static const uint8_t MODE_CURSOR_RGHT_INCR = 0x02;
			
			static const uint8_t DISPLAY_BLINK_OFF = 0x00;
			static const uint8_t DISPLAY_BLINK_ON = 0x01;
			static const uint8_t DISPLAY_CURSOR_OFF = 0x00;
			static const uint8_t DISPLAY_CURSOR_ON = 0x02;
			static const uint8_t DISPLAY_OFF = 0x00;
			static const uint8_t DISPLAY_ON = 0x04;
			
			static const uint8_t SHIFT_LEFT = 0x00;
			static const uint8_t SHIFT_RIGHT = 0x04;
			static const uint8_t SHIFT_CURSOR = 0x00;
			static const uint8_t SHIFT_SCREEN = 0x08;
			
			static const uint8_t FUNCTION_SIZE_5x8 = 0x00;
			static const uint8_t FUNCTION_SIZE_5x11 = 0x04;
			static const uint8_t FUNCTION_LINE_1 = 0x00;
			static const uint8_t FUNCTION_LINE_2 = 0x08;


		/*
		 * Creates a new HD44780 module in 3-wire mode.
		 * The port and pin are that are connected to E on the module.
		 * The port and pin are that are connected to RS on the module.
		 * MOSI must be connected to DI on the shift register.
		 * SCK must be connected to SCK on the shift register.
		 * SS must either be configured as an output or if configured as in input then in must be held high.
		 */
		Hd44780(volatile uint8_t *e_port, uint8_t e_pin, volatile uint8_t *rs_port, uint8_t rs_pin, volatile uint8_t *spi_port, uint8_t mosi_pin, uint8_t sclk_pin, uint8_t function);
	
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
		 * Writes sz bytes to either CGRAM or DDRAM.  The starting address must be set in advance and no bounds checking is done.
		 */
		void setText(char* text, uint8_t sz);

		/*
		 * Writes sz bytes to either CGRAM or DDRAM.  The starting address must be set in advance and no bounds checking is done.
		 */
		void setBytes(uint8_t bytes[], uint8_t sz);
	};
}

#endif