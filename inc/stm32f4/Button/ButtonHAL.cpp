#include "ButtonHAL.h"

using namespace digitalcave;

ButtonHAL::ButtonHAL(GPIO_TypeDef* port, uint16_t pin, uint16_t pressTime, uint16_t releaseTime, uint16_t longPressTime, uint16_t repeatPressTime):
		Button(pressTime, releaseTime, longPressTime, repeatPressTime),
		port(port),
		pin(pin)
{
	;	//Be sure the pin is already defined as input, with a pullup
}

uint8_t ButtonHAL::read(){
	return (HAL_GPIO_ReadPin(port, pin)) == GPIO_PIN_RESET ? 1 : 0;
}
