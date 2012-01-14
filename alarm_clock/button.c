#include "button.h"

void button_init(){
	//Pullups on all the button pins.
	BUTTON_PORT |= _BV(BUTTON_TIME_PIN) | _BV(BUTTON_ALARM_PIN) 
					| _BV(BUTTON_SLEEP_PIN) | _BV(BUTTON_HOUR_PIN) 
					| _BV(BUTTON_MINUTE_PIN) | _BV(BUTTON_SNOOZE_PIN);
}

uint8_t button_read(){
	//We invert PINB so that we don't have to worry about inverse logic due to pullups.
	uint8_t result = 0x00;
	if (~PINB & _BV(BUTTON_TIME_PIN)) result |= BUTTON_TIME;
	if (~PINB & _BV(BUTTON_ALARM_PIN)) result |= BUTTON_ALARM;
	if (~PINB & _BV(BUTTON_SLEEP_PIN)) result |= BUTTON_SLEEP;
	if (~PINB & _BV(BUTTON_HOUR_PIN)) result |= BUTTON_HOUR;
	if (~PINB & _BV(BUTTON_MINUTE_PIN)) result |= BUTTON_MINUTE;
	if (~PINB & _BV(BUTTON_SNOOZE_PIN)) result |= BUTTON_SNOOZE;
	
	return result;
}
