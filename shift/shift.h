#ifndef SHIFT_H
#define SHIFT_H

#include <avr/io.h>

/*
 * Shift register library - this is designed for 74595 and compatible shift registers.  You 
 * need three wires to control the register from the AVR: 
 *  Data Out (74595 pin 14)
 *  Clock Out (74595 pin 11)
 *  Latch Out (74595 pin 12)
 *
 * Shift Register Pinout:
 * Pin 1 - 7: Outputs B - H
 * Pin 8: GND
 * Pin 9: Serial Output (Chain this to pin 14 if you are linking multiple registers)
 * Pin 10: /Shift Register Clear (Connect to VCC)
 * Pin 11: Connect to AVR Clock Out  <-----
 * Pin 12: Connect to AVR Latch Out  <-----
 * Pin 13: /Output Enable (Connect to GND)
 * Pin 14: Connect to AVR Data Out or previous shift register if chaining  <-----
 * Pin 15: Output A
 * Pin 16: VCC
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
