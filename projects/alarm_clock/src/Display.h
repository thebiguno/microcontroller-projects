#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>

#include <font/font.h>

#include <SPIStreamAVR.h>
#include <MAX7219.h>

#include "Buffer.h"
#include "light.h"
#include "State.h"
#include "font/f_3x5.h"
#include "font/f_5x8.h"
#include "font/f_icon.h"

#define LIGHT_PORT			PORTD
#define LIGHT_Y_PIN			PORTD5
#define LIGHT_N_PIN			PORTD6
#define LIGHT_B_PIN			PORTD7

#define FLASH_TIMER_ON		0x05

#define NO_FLASH			0xFF

void display_init();

void display_update();

#endif
