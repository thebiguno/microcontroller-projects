#ifndef BUTTON_H
#define BUTTON_H

#include <avr/io.h>

#define BUTTON_PORT					PORTB
#define BUTTON_ALARM_PIN			PINB0
#define BUTTON_TIME_PIN				PINB1
#define BUTTON_MINUTE_PIN			PINB2
#define BUTTON_HOUR_PIN				PINB3
#define BUTTON_SLEEP_PIN			PINB4
#define BUTTON_SNOOZE_PIN			PINB5

//Flags in the return value of button_read()
#define BUTTON_TIME					_BV(0)
#define BUTTON_ALARM				_BV(1)
#define BUTTON_SLEEP				_BV(2)
#define BUTTON_HOUR					_BV(3)
#define BUTTON_MINUTE				_BV(4)
#define BUTTON_SNOOZE				_BV(5)

/*
 * Init hardware for reading buttons
 */
void button_init();

/*
 * Read the buttons
 */
uint8_t button_read();

#endif