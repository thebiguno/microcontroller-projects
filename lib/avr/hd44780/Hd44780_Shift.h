#ifndef HD44780_SHIFT_H
#define HD44780_SHIFT_H

#include <avr/io.h>
#include <stdlib.h>

#include "Hd44780.h"

namespace digitalcave {

	class Hd44780_Shift : public Hd44780 {
	
		protected:
			virtual void cmd(uint8_t cmd);

		public:
			/*
			 * Creates a new HD44780 module in 3-wire mode.
			 * The port and pin are that are connected to E on the module.
			 * The port and pin are that are connected to RS on the module.
			 * MOSI must be connected to DI on the shift register.
			 * SCK must be connected to SCK on the shift register.
			 * SS must either be configured as an output or if configured as in input then in must be held high.
			 */
			Hd44780_Shift(volatile uint8_t *e_port, uint8_t e_pin, volatile uint8_t *rs_port, uint8_t rs_pin, volatile uint8_t *spi_port, uint8_t mosi_pin, uint8_t sclk_pin, uint8_t function);

			virtual void setByte(uint8_t b);
	};
}

#endif