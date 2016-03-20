/*
 * Stm32 HAL implementation of Button library.
 */

#ifndef BUTTON_HAL_H
#define BUTTON_HAL_H

#include "stm32f4xx_hal.h"
#include "../../common/Button/Button.h"

namespace digitalcave {

	class ButtonHAL : public Button {
		private:
			GPIO_TypeDef* port;
			uint16_t pin;

			
		public:
			ButtonHAL(GPIO_TypeDef* port, uint16_t pin, uint16_t pressTime, uint16_t releaseTime, uint16_t longPressTime, uint16_t repeatPressTime);
			
			uint8_t read();
			
			using Button::read; // Allow other overloaded functions from superclass to show up in subclass.
	};
}

#endif