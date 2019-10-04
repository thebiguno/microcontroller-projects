/*
 * Si4703 library.  High level logic taken from Sparkfun Arduino library (https://github.com/sparkfun/SparkFun_Si4703_Arduino_Library/tree/master/src),
 * and adapted into C++ code compatible with digitalcave microcontroller libraries by Wyatt Olson.
 */

#include "MAX7219.h"

#include <stdio.h>

using namespace digitalcave;

MAX7219::MAX7219(Stream* spi, uint8_t display_count) :
	spi(spi),
	display_count(display_count) {

	write_command(MAX7219_SCAN_LIMIT, 0x07, display_count);
	write_command(MAX7219_DECODE, 0x00, display_count);
	write_command(MAX7219_SHUTDOWN, 0x01, display_count);
	write_command(MAX7219_DISPLAY_TEST, 0x00, display_count);
	for (uint8_t i = 0; i < 8; i++){
		write_command(i + 1, 0x00, display_count);
	}
	write_command(MAX7219_INTENSITY, 0x00, display_count);
}

void MAX7219::set_brightness(uint8_t brightness){
	if (brightness > 0x0F){
		brightness = 0x0F;
	}
	write_command(MAX7219_INTENSITY, brightness, display_count);
}

void MAX7219::write_command(uint8_t reg, uint8_t data, uint8_t repeat){
	PORTB |= _BV(DD_SS);

	for (uint8_t i = 0; i < repeat; i++){
		spi->write(reg);
		spi->write(data);
	}

	PORTB &= ~_BV(DD_SS);
	PORTB |= _BV(DD_SS);
}

void MAX7219::write_buffer(uint8_t* buffer){
	for (uint8_t y = 0; y < 8; y++){
		write_row(y, buffer + (y * display_count));
	}
}

void MAX7219::write_row(uint8_t row, uint8_t* buffer){
	PORTB |= _BV(DD_SS);

	for (int8_t i = display_count - 1; i >= 0; i--){
		spi->write(row + 1);
		spi->write(buffer[i]);
	}

	PORTB &= ~_BV(DD_SS);
	PORTB |= _BV(DD_SS);
}
