/*
 * Software implementation of TX side of Serial library.  RX always returns 'no data'.  This can be
 * used for sending data on any pin without the need for timers, etc.
 *
 * Note: This blocks and stops interrupts until each character has finished sending.  Use it wisely.
 *
 * Tested and calibrated on 16MHz and 8MHz, at 9600, 19200, 38400, 57600, and 115200 (at 16Mhz only).
 * Other clock speeds and baud rates are not currently supported.
 *
 * Even with supported speeds, things tend to be more accurate at lower speeds (9600, etc).  Be conservative.
 */

#ifndef SPI_STREAM_AVR_H
#define SPI_STREAM_AVR_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <dcutil/spi.h>

#include <Stream.h>

namespace digitalcave {

	class SPIStreamAVR : public Stream {
//		private:


		public:
			//Initialize specifying baud rate and all other optional parameters
			SPIStreamAVR();

			// Implementation of virtual functions declared in superclass
			uint8_t read(uint8_t *b);
			uint8_t write(uint8_t data);

			using Stream::read; // Allow other overloaded functions from superclass to show up in subclass.
			using Stream::write; // Allow other overloaded functions from superclass to show up in subclass.
	};
}

#endif
