/*
 * Software implementation of TX side of Serial library.  RX always returns 'no data'.  This can be
 * used for sending data on any pin without the need for timers, etc.
 * 
 * Note: This blocks and stops interrupts until each character has finished sending.  Use it wisely.
 * 
 * Tested and calibrated on 16MHz and 8MHz, at 9600, 19200, 38400, 57600, and 115200 (at 16Mhz only).
 * Other clock speeds and baud rates are not currently supported.
 */

#ifndef SOFTWARE_SERIAL_AVR_H
#define SOFTWARE_SERIAL_AVR_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include <Serial.h>

namespace digitalcave {

	class SoftwareSerialAVR : public Serial {
		private:
			uint32_t baud;
			volatile uint8_t* port;
			uint8_t pin;
			
			uint8_t delayCycles;
			uint8_t delayCyclesLong;
			
			void delay();
			
		public:
			//Initialize specifying baud rate and all other optional parameters
			SoftwareSerialAVR(volatile uint8_t* port, uint8_t pin, uint32_t baud);
			
			// Implementation of virtual functions declared in superclass
			uint8_t read(uint8_t *b);
			uint8_t write(uint8_t data);
			
			using Stream::read; // Allow other overloaded functions from superclass to show up in subclass.
			using Stream::write; // Allow other overloaded functions from superclass to show up in subclass.
	};
}

#endif