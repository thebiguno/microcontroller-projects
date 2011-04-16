#ifndef GLCD_H
#define GLCD_H

#include <avr/io.h>
#include <util/delay.h>

#define CTRL_CLK 1

/*
 * Graphical LCD library, for Newhaven Display NHD‐C12832A1Z‐NSW‐BBW‐3V3 
 * (Digikey part #NHD-C12832A1Z-NSW-BBW-3V3-ND); 128x32 LCD display, SPI interface.
 *
 * There are 4 wires that you care about: 
 *  SDA (Serial Data)
 *  SCL (Serial Clock)
 *  A0 (Select Registers)
 *  CS1B (Chip Select)
 * These must be initialized with a port and pin in the init method.
 */

/*
 * Initializes the library to use the specified ports and pins for the required
 * pins.  You must call this before you can use any other functions.
 * 
 * You need to pass the ports by reference - e.g. 
 * glcd_init(&PORTC, PORTC0, &PORTC, PORTC1, &PORTC, PORTC2, &PORTC, PORTC3 &PORTC, PORTC4).
 */
void glcd_init(volatile uint8_t *data_port, uint8_t data_pin, 
		volatile uint8_t *clock_port, uint8_t clock_pin, 
		volatile uint8_t *select_register_port, uint8_t select_register_pin,
		volatile uint8_t *select_chip_port, uint8_t select_chip_pin,
		volatile uint8_t *reset_port, uint8_t reset_pin);

void do_something();
#endif
