#ifndef HD44780_DIRECT_H
#define HD44780_DIRECT_H

#include <avr/io.h>
#include <stdlib.h>

#include "Hd44780.h"

namespace digitalcave {

	class Hd44780_Direct : public Hd44780 {
	
		protected:
			virtual void cmd(uint8_t cmd);

		public:
			/*
			 * Creates a new HD44780 module in 8 bit direct drive mode.
			 */
			Hd44780_Direct(uint8_t function);

			virtual void setByte(uint8_t b);
	};
}

#endif