/*
Copyright 2014 Warren Janssens <warren.janssens@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


This work is heavily based on initial firmware for Ergodox keyboard
Copyright (c) 2013 Oleg Kostyuk <cub.uanic@gmail.com>
Project located at <https://github.com/cub-uanic/tmk_keyboard>
which is in turn heavily based on initial firmware for Ergodox keyboard
Copyright (c) 2012, 2013 Ben Blazak <benblazak.dev@gmail.com>
Released under The MIT License (see "doc/licenses/MIT.md")
Project located at <https://github.com/benblazak/ergodox-firmware>

Most used files are located at
<https://github.com/benblazak/ergodox-firmware/tree/partial-rewrite/firmware/keyboard/ergodox/controller>

*/

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "action.h"
#include "command.h"
#include "print.h"
#include "debug.h"
#include "ergo60.h"

bool i2c_initialized = 0;

void init_ergo60(void)
{
	// TODO change to Atmega 644 code
    // keyboard LEDs (see "PWM on ports OC1(A|B|C)" in "teensy-2-0.md")
    TCCR1A = 0b10101001;  // set and configure fast PWM
    TCCR1B = 0b00001001;  // set and configure fast PWM

    // (tied to Vcc for hardware convenience)
    DDRB  &= ~(1<<4);  // set B(4) as input
    PORTB &= ~(1<<4);  // set B(4) internal pull-up disabled

    // unused pins - C7, D4, D5, D7, E6
    // set as input with internal pull-ip enabled
    DDRC  &= ~(1<<7);
    DDRD  &= ~(1<<7 | 1<<5 | 1<<4);
    DDRE  &= ~(1<<6);
    PORTC |=  (1<<7);
    PORTD |=  (1<<7 | 1<<5 | 1<<4);
    PORTE |=  (1<<6);
}

void ergo60_blink_all_leds(void)
{
    ergo60_led_all_off();
    ergo60_led_all_set(LED_BRIGHTNESS_HI);
    ergo60_led_all_on();
    _delay_ms(333);
    ergo60_led_all_off();
}

