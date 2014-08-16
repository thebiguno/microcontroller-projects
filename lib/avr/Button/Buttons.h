#ifndef BUTTONS_H
#define BUTTONS_H

#include <avr/io.h>
#include <stdlib.h>

namespace digitalcave {
	/*
	 * This library is suitable for general SPST-NO buttons and switches that have a minimum 100 ms press time.
	 */
	class Buttons {

	private:
		volatile uint8_t *port;
		volatile uint8_t *pin;
		uint8_t pins_bv;
		uint8_t current;
		uint8_t last;
		uint8_t press;
		uint8_t release;

	public:
		/*
		 * Creates a new Buttons class that can debounce the selected pins of a given port.
		 * The selected pins are configured for input and pull ups are enabled making this library suitable only for active-low configuration.
		 */
		Buttons(volatile uint8_t *port, uint8_t pins);
		
		/*
		 * Samples the pins once.
		 */
		void sample();
		
		/*
		 * Samples the pins 8 times at 12 ms intervals ensuring that buttons remain open or closed for 100 ms before integrating the samples.
 		 * Returns the mask of buttons that are pressed.
		 * Any pin that is pressed will have its bit set.
		 */
		uint8_t poll();
		
		/*
		 * Integrate the press and release samples and update the current state.
 		 * Returns the mask of buttons that are pressed.
		 */
		uint8_t integrate();
		
		/*
 		 * Returns the mask of buttons that are pressed, as it was the last time the samples were integrated.
		 */
		uint8_t pressed();
		
		/*
		 * Returns the mask of buttons that have changed state since the last time this method was called.
		 * Any pin that has changed since the last call to this method will have its bit set.
		 */
		uint8_t changed();
	};
}

#endif
