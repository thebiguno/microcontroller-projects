/*
 * AVR implementation of Button library.
 */

#ifndef BUTTON_TEENSY_H
#define BUTTON_TEENSY_H

#include <avr/interrupt.h>
#include <avr/io.h>

#include "../../common/Button/Button.h"

namespace digitalcave {

	class ButtonTeensy : public Button {
		private:
			uint8_t pin;
			
		public:
			ButtonTeensy(uint8_t pin, uint16_t pressTime, uint16_t releaseTime, uint16_t longPressTime, uint16_t repeatPressTime);
			
			uint8_t read();
	};
}

#endif