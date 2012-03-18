#ifndef BUTTON_H
#define BUTTON_H

#include <avr/io.h>
#include <stdlib.h>

/*
 * C++ implementation of button reading / debounce library.  It handles a single button,
 * and requires a poll() to keep state.  Probably not suitable for production code,
 * but is a simple POC for C++.
 */
class Button {
	private:
		uint8_t debounceCount;				//The debounce count 
		uint8_t counter;					//A counter to track the button debounce
		volatile uint8_t *port;				//The PORT for the button
		volatile uint8_t *pin;				//The PIN for the button
		uint8_t idx;						//The index for the button
		uint8_t lastState;					//The last state, used for change events

	public:
		/*
		 * Constructor for a button at specified port / pin.  The
		 * button has to read 'pressed' for this many iterations 
		 * for it to report as pressed.
		 *
		 * To initialize this, you must pass in references to the port and pin:
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
		 *    Button b(&PORTB, 1, 16);
		 */
		Button(volatile uint8_t *port, uint8_t idx, uint8_t debounceCount);
		
		/*
		 * Polls the button.  You should do this as frequently as possible.  
		 * A button has to read 'pressed' for 'debugCount' iterations
		 * in a row before it reports as pressed in isPressed();
		 */
		void poll();
		
		/*
		 * Returns zero if the debounced button is not pressed, non-zero if it 
		 * is pressed.
		 */
		uint8_t isPressed();
		
		/*
		 * Returns non-zero if the button has changed from the last time isPressed()
		 * was called, zero if it is unchanged.
		 */
		uint8_t isChanged();
} ;

#endif
