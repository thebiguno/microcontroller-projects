#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <font/font.h>

namespace digitalcave {

	class Buffer {
		private:
			uint8_t* data;	//Number of displays * number of lines

			uint8_t width;
			uint8_t height;

		public:

			/*
			 * Initializes the channel
			 */
			Buffer(uint8_t width, uint8_t height);

			void clear();

			void set_pixel(int16_t x, int16_t y, uint8_t value);

			int16_t write_char(char c, font_t font, int16_t x, int16_t y);

			int16_t write_string(char* text, font_t font, int16_t x, int16_t y);
			int16_t write_string(const char* text, font_t font, int16_t x, int16_t y);

			uint8_t* get_data();
	};
}

#endif
