#include "button.h"

void button_init(){
	//Pullups on all the button pins.
	BUTTON_PORT |= _BV(BUTTON_TIME_PIN) | _BV(BUTTON_ALARM_PIN) 
					| _BV(BUTTON_SLEEP_PIN) | _BV(BUTTON_HOUR_PIN) 
					| _BV(BUTTON_MINUTE_PIN) | _BV(BUTTON_SNOOZE_PIN);
}

uint8_t button_read(){
	//We invert PINB so that we don't have to worry about inverse logic due to pullups.
	return ~BUTTON_PIN & (_BV(BUTTON_TIME_PIN) | _BV(BUTTON_ALARM_PIN)
			| _BV(BUTTON_SLEEP_PIN) | _BV(BUTTON_HOUR_PIN) 
			| _BV(BUTTON_MINUTE_PIN) | _BV(BUTTON_SNOOZE_PIN));
}
