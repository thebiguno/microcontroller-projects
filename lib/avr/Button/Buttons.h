#ifndef BUTTONS_H
#define BUTTONS_H

#include <avr/io.h>
#include <stdlib.h>

namespace digitalcave {
	class Buttons {

	private:
		volatile uint8_t *port;
		uint8_t pins_bv;
		uint8_t state;
		uint8_t lastState;
		double us;

	public:
		/*
		 * Creates a new Buttons class that can debounce the selected pins of a given port.
		 * The sample size is always 8, with us microseconds between samples.
		 * The selected pins are configured for input and pull ups are enabled making this library suitable only for active-low configuration.
		 * There is a delay of us microseconds between samples.  The allowable values for this argument are described in the documentation for _delay_us here:
		 * http://www.nongnu.org/avr-libc/user-manual/group__util__delay.html
		 */
		Buttons(volatile uint8_t *port, uint8_t pins, double us);
		
		/*
		 * Samples the port 8 times and returns the mask of buttons that are pressed.
		 * Any pin that is pressed will have the value 1.
		 */
		uint8_t poll();
			
		/*
		 * Returns the mask of the buttons that are pressed.
		 * Any pin that is pressed will have the value 1.
		 */
		uint8_t pressed();
		
		/*
		 * Returns the mask of buttons that have changed state since the last time pressed was called.
		 * Any pin that changed since the last call to changed will have the value 0.
		 */
		uint8_t changed();
	};
}
