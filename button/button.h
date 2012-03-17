#ifndef BUTTON_H
#define BUTTON_H

#include <avr/io.h>
#include <stdlib.h>

/*
 * C++ implementation of button reading / debounce library.  It can handle an arbitrary 
 * number of buttons, and will debounce each and return state when queried.
 */
class Button {
	private:
		uint8_t size;				//The number of buttons
		uint8_t count;				//The debounce count 
		uint8_t *counters;			//An array of counters to track each button debounce
		volatile uint8_t **ports;	//The ports for each button
		uint8_t *pins;				//The pins for each button

	public:
		/*
		 * Constructor for a group of buttons with a specified count; the
		 * button has to read 'pressed' for this many iterations before it
		 * counts as pressed.
		 *
		 * ports and pins are required; size defaults to 1 and count defaults to 8.
		 *
		 * To initialize this, you must pass in references to each of the PORTs, DDRs, and
		 * the values of each pin.  You can do it like this:
		 * 
		 *    volatile uint8_t *ports[6];
		 *    ports[0] = &PORTB;
		 *    ports[1] = &PORTD;
		 *    ....
		 *    ports[5] = &PORTD;
		 *
		 *    uint8_t pins[8];
		 *    pins[0] = 1;
		 *    ....
		 *    pins[5] = 3;
		 *
		 *    new Button(ports, pins, 6);
		 */
		Button(volatile uint8_t **ports, uint8_t *pins, uint8_t size, uint8_t count);
		
		/*
		 * Polls all defined buttons.  You should do this as frequently
		 * as possible.  A button has to read 'pressed' for 'count' iterations
		 * in a row before it reports as pressed in isPressed();
		 */
		void poll();
		
		/*
		 * Returns zero if the debounced button is not pressed, non-zero if it 
		 * is pressed.
		 */
		uint8_t isPressed(uint8_t index);
	
} button;

#endif
