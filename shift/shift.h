#ifndef SHIFT_H
#define SHIFT_H

#include <avr/io.h>

/*
 * Shift register library - this is designed for 74595 and compatible shift registers.  You 
 * need three wires to control the register from the AVR: 
 *  Data Out (74595 pin 14)
 *  Clock Out (74595 pin 11)
 *  Latch Out (74595 pin 12)
 * You need to connect /Shift Register Clear (pin 10) to VCC and /Output Enable (pin 13) to GND
 */

/*
 * Initializes the library to use the specified ports and pins for data out, clock, and latch.  You can
 * call this function again to change to different values.
 * 
 * You need to pass the ports by reference - e.g. shift_init(&PORTC, PORTC0, &PORTC, PORTC1, &PORTC, PORTC2).
 */
void shift_init(volatile uint8_t *data_port, uint8_t data_pin, 
		volatile uint8_t *clock_port, uint8_t clock_pin, 
		volatile uint8_t *latch_port, uint8_t latch_pin);

/*
 * Sends one byte of data to the shift register.  Must have called shift_init() first.
 */
uint8_t shift_out(uint8_t data);

#endif
