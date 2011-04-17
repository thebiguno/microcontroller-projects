#ifndef GLCD_ST7565R_H
#define GLCD_ST7565R_H

#include <util/delay.h>
#include "glcd.h"

//Half clock period, in micro seconds.  1 should be fine.
#define CTRL_CLK 1

/*
 * Graphical LCD library, for ST7565R LCD driver in SPI mode.
 * In particular, this is used for Newhaven Display NHD‐C12832A1Z‐NSW‐BBW‐3V3 
 * (Digikey part #NHD-C12832A1Z-NSW-BBW-3V3-ND); 128x32 LCD display.
 *
 * There are 5 wires that you care about: 
 *  SDA (Serial Data)
 *  SCL (Serial Clock)
 *  A0 (Select Registers)
 *  CS1B (Chip Select)
 *  RESET (Reset)
 * These must be initialized with a port and pin in the init method.
 */

/*
 * Initializes the library to use the specified ports and pins for the required
 * pins.  You must call this before you can use any other functions.
 * 
 * You need to pass the ports by reference - e.g. 
 * glcd_init(&PORTC, PORTC0, &PORTC, PORTC1, &PORTC, PORTC2, &PORTC, PORTC3 &PORTC, PORTC4).
 */
void st7565r_init(volatile uint8_t *data_port, uint8_t data_pin, 
		volatile uint8_t *clock_port, uint8_t clock_pin, 
		volatile uint8_t *a0_port, uint8_t a0_pin,
		volatile uint8_t *reset_port, uint8_t reset_pin);



#endif
