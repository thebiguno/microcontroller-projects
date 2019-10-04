#ifndef MAX7219_h
#define MAX7219_h

#include <avr/io.h>
#include <stdlib.h>
#include <math.h>

#include <SPIStreamAVR.h>
#include <dctypes.h>
#include <dcutil/delay.h>
#include <dcutil/spi.h>


#define MAX7219_DECODE        0x09
#define MAX7219_INTENSITY     0x0A
#define MAX7219_SCAN_LIMIT    0x0B
#define MAX7219_SHUTDOWN      0x0C
#define MAX7219_DISPLAY_TEST  0x0F

//Inits the MAX7219 control object
void max7219_init(uint8_t display_count);

void max7219_write_command(uint8_t register, uint8_t data, uint8_t repeat);

void max7219_write_row(uint8_t row, uint8_t* data);

void max7219_write_buffer(uint8_t* buffer);

void max7219_set_brightness(uint8_t brightness);

#endif
