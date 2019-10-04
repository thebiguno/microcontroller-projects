#include "max7219.h"
/*
 * MAX7219 library, using an SPI stream for actual communication.  Written by Wyatt Olson.
 *
 */

using namespace digitalcave;

static SPIStreamAVR* _spi = NULL;
static uint8_t _display_count = 0;

void max7219_init(uint8_t display_count){
	if (_spi == NULL){
		_spi = new SPIStreamAVR();
	}
	_display_count = display_count;

	max7219_write_command(MAX7219_SCAN_LIMIT, 0x07, _display_count);
	max7219_write_command(MAX7219_DECODE, 0x00, _display_count);
	max7219_write_command(MAX7219_SHUTDOWN, 0x01, _display_count);
	max7219_write_command(MAX7219_DISPLAY_TEST, 0x00, _display_count);
	for (uint8_t i = 0; i < 8; i++){
		max7219_write_command(i + 1, 0x00, _display_count);
	}
	max7219_write_command(MAX7219_INTENSITY, 0x00, _display_count);
}

void max7219_set_brightness(uint8_t brightness){
	if (brightness > 0x0F){
		brightness = 0x0F;
	}
	max7219_write_command(MAX7219_INTENSITY, brightness, _display_count);
}

void max7219_write_buffer(uint8_t* buffer){
	for (uint8_t y = 0; y < 8; y++){
		max7219_write_row(y, buffer + (y * _display_count));
	}
}

void max7219_write_row(uint8_t row, uint8_t* buffer){
	PORTB |= _BV(DD_SS);

	for (int8_t i = _display_count - 1; i >= 0; i--){
		_spi->write(row + 1);
		_spi->write(buffer[i]);
	}

	PORTB &= ~_BV(DD_SS);
	PORTB |= _BV(DD_SS);
}

void max7219_write_command(uint8_t reg, uint8_t data, uint8_t repeat){
	PORTB |= _BV(DD_SS);

	for (uint8_t i = 0; i < repeat; i++){
		_spi->write(reg);
		_spi->write(data);
	}

	PORTB &= ~_BV(DD_SS);
	PORTB |= _BV(DD_SS);
}
