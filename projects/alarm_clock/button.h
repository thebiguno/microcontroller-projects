#ifndef BUTTON_H
#define BUTTON_H

#include <avr/io.h>

#define BUTTON_PORT					PORTB
#define BUTTON_PIN					PINB
#define BUTTON_ALARM_PIN			PINB0
#define BUTTON_TIME_PIN				PINB1
#define BUTTON_MINUTE_PIN			PINB2
#define BUTTON_HOUR_PIN				PINB3
#define BUTTON_SLEEP_PIN			PINB4
#define BUTTON_SNOOZE_PIN			PINB5

//Flags in the return value of button_read().  For convenience and speed
// we make sure that these are the same as the pins above.
#define BUTTON_ALARM				_BV(BUTTON_ALARM_PIN)
#define BUTTON_TIME					_BV(BUTTON_TIME_PIN)
#define BUTTON_MINUTE				_BV(BUTTON_MINUTE_PIN)
#define BUTTON_HOUR					_BV(BUTTON_HOUR_PIN)
#define BUTTON_SLEEP				_BV(BUTTON_SLEEP_PIN)
#define BUTTON_SNOOZE				_BV(BUTTON_SNOOZE_PIN)

/*
 * Init hardware for reading buttons
 */
void button_init();

/*
 * Read the buttons
 */
uint8_t button_read();

#endif