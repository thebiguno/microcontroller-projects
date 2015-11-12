/*
 * AVR implementation of Button library.
 */

#ifndef BUTTON_AVR_H
#define BUTTON_AVR_H

#include <avr/interrupt.h>
#include <avr/io.h>

#include "../../common/Button/Button.h"

namespace digitalcave {

	class ButtonAVR : public Button {
		private:
			volatile uint8_t* port;
			uint8_t pin;

			
		public:
			ButtonAVR(volatile uint8_t* port, uint8_t pin, uint16_t pressTime, uint16_t releaseTime, uint16_t longPressTime, uint16_t repeatPressTime);
			
			uint8_t read();
			
			using Button::read; // Allow other overloaded functions from superclass to show up in subclass.
	};
}

#endif