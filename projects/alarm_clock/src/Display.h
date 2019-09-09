#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>

#include <font/font.h>

#include <SPIStreamAVR.h>
#include <MAX7219.h>

#include "Buffer.h"
#include "Light.h"
#include "State.h"
#include "font/f_icon.h"
#include "font/f_3x3.h"
#include "font/f_3x5.h"
#include "font/f_5x8.h"

#define LIGHT_PORT			PORTD
#define LIGHT_Y_PIN			PORTD5
#define LIGHT_N_PIN			PORTD6
#define LIGHT_B_PIN			PORTD7

#define FLASH_TIMER_ON		0x05

namespace digitalcave {

	class Display {
		private:
			SPIStreamAVR spi;
			MAX7219 display;
			Buffer buffer;

			uint8_t flash_timer = 0;

			char temp[10];

			void write_time(dc_time_t time, uint8_t flash_field);

			void write_date(dc_time_t time, uint8_t flash_field);

		public:
			Display();

			void update(State state);
	};
}

#endif
