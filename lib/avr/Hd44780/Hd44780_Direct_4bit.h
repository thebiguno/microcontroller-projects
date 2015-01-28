/*
 * Driver for HD44780 in 4 bit mode using direct pin access.
 * Defaults to use PORTB0 for RS, and PORTB1 for E,PORTB2..5 
 * for data pins.  Use the alternate constructor to define
 * each pin individually.
 */

#ifndef HD44780_DIRECT_4BIT_H
#define HD44780_DIRECT_4BIT_H

#include <avr/io.h>
#include <stdlib.h>

#include "Hd44780.h"

namespace digitalcave {

	class Hd44780_Direct_4bit : public Hd44780 {
		private:
			volatile uint8_t *port_rs; 
			uint8_t pin_rs;
			volatile uint8_t *port_e; 
			uint8_t pin_e;
			volatile uint8_t *port_d4; 
			uint8_t pin_d4;
			volatile uint8_t *port_d5; 
			uint8_t pin_d5;
			volatile uint8_t *port_d6; 
			uint8_t pin_d6;
			volatile uint8_t *port_d7; 
			uint8_t pin_d7;
			
			void cmd(uint8_t cmd);
			void write_byte(uint8_t b, uint8_t mode);
			void write_nibble(uint8_t b, uint8_t mode);
			
		public:
			Hd44780_Direct_4bit(uint8_t function);
			
			Hd44780_Direct_4bit(uint8_t function, 
								volatile uint8_t *port_rs, uint8_t pin_rs,
								volatile uint8_t *port_e, uint8_t pin_e,
								volatile uint8_t *port_d4, uint8_t pin_d4,
								volatile uint8_t *port_d5, uint8_t pin_d5,
								volatile uint8_t *port_d6, uint8_t pin_d6,
								volatile uint8_t *port_d7, uint8_t pin_d7);

			virtual void setByte(uint8_t b);
	};
}

#endif