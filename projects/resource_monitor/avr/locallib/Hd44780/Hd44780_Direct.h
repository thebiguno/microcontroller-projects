#ifndef HD44780_DIRECT_H
#define HD44780_DIRECT_H

#include <avr/io.h>
#include <stdlib.h>

#include "Hd44780.h"

namespace digitalcave {

	class Hd44780_Direct : public Hd44780 {
		private:
			volatile uint8_t *port_rs; 
			uint8_t pin_rs;
			volatile uint8_t *port_e; 
			uint8_t pin_e;
			volatile uint8_t *port_d0; 
			uint8_t pin_d0;
			volatile uint8_t *port_d1; 
			uint8_t pin_d1;
			volatile uint8_t *port_d2; 
			uint8_t pin_d2;
			volatile uint8_t *port_d3; 
			uint8_t pin_d3;
			volatile uint8_t *port_d4; 
			uint8_t pin_d4;
			volatile uint8_t *port_d5; 
			uint8_t pin_d5;
			volatile uint8_t *port_d6; 
			uint8_t pin_d6;
			volatile uint8_t *port_d7; 
			uint8_t pin_d7;
			
			void cmd(uint8_t cmd);

		public:
			/*
			 * Creates a new HD44780 module in 8 bit direct drive mode.
			 */
			Hd44780_Direct(uint8_t function);
			/* TODO add support for specific pins
			Hd44780_Direct(uint8_t function, 
								volatile uint8_t *port_rs, uint8_t pin_rs,
								volatile uint8_t *port_e, uint8_t pin_e,
								volatile uint8_t *port_d0, uint8_t pin_d0,
								volatile uint8_t *port_d1, uint8_t pin_d1,
								volatile uint8_t *port_d2, uint8_t pin_d2,
								volatile uint8_t *port_d3, uint8_t pin_d3,
								volatile uint8_t *port_d4, uint8_t pin_d4,
								volatile uint8_t *port_d5, uint8_t pin_d5,
								volatile uint8_t *port_d6, uint8_t pin_d6,
								volatile uint8_t *port_d7, uint8_t pin_d7);
			*/
			virtual void setByte(uint8_t b);
	};
}

#endif