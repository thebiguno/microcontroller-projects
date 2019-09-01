#ifndef MAX7219_h
#define MAX7219_h

#include <avr/io.h>
#include <stdlib.h>
#include <math.h>

#include <dctypes.h>
#include <dcutil/delay.h>
#include <dcutil/spi.h>

#include <Stream.h>

#define MAX7219_DECODE        0x09
#define MAX7219_INTENSITY     0x0A
#define MAX7219_SCAN_LIMIT    0x0B
#define MAX7219_SHUTDOWN      0x0C
#define MAX7219_DISPLAY_TEST  0x0F

namespace digitalcave {

	class MAX7219 {
		private:
			Stream* spi;
			uint8_t display_count;

			void write_command(uint8_t register, uint8_t data, uint8_t repeat);

		public:
			//Inits the MAX7219 control object, passing in an SPI stream
			MAX7219(Stream* spi, uint8_t display_count);

			void write_row(uint8_t row, uint8_t* data);

			void write_buffer(uint8_t* buffer);

	};
}
#endif
