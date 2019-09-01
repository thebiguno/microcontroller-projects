#ifndef ALARMCLOCK_H
#define ALARMCLOCK_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>

//#include <Si4703.h>
#include <font/font.h>
#include <timer/timer.h>

#include <SPIStreamAVR.h>
#include <MAX7219.h>

#include <SerialUSB.h>

#include "Buffer.h"
#include "font/f_clockface.h"


#ifndef CHANNEL_COUNT
#define CHANNEL_COUNT					4
#endif

#if CHANNEL_COUNT > 6
#error The software does not currently support more than 6 channels (not enough ADCs on an ATmega32u4).
#endif

void buffer_clear(uint8_t* buffer);
void buffer_draw_text(uint8_t* buffer, char* text, font_t font, uint8_t x, uint8_t y);
void buffer_draw_character(uint8_t* buffer, char c, font_t font, uint8_t x, uint8_t y);
void buffer_set_pixel(uint8_t* buffer, uint8_t x, uint8_t y, uint8_t value);

#endif
