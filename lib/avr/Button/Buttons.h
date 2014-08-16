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

	public:
		/*
		 * Creates a new Buttons class that can debounce the selected pins of a given port.
		 * The selected pins are configured for input and pull ups are enabled making this library suitable only for active-low configuration.
		 */
		Buttons(volatile uint8_t *port, uint8_t pins);
		
		/*
		 * Samples the pins 8 times every 12 ms ensuring that buttons remain closed for 100 ms.
 		 * Returns the mask of buttons that are pressed.
		 * Any pin that is pressed will have its bit set.
		 */
		uint8_t poll();
			
		/*
		 * Returns the mask of the buttons that are pressed, as determined by the last call to poll.
		 * Any pin that is pressed will have its bit set.
		 */
		uint8_t pressed();
		
		/*
		 * Returns the mask of buttons that have changed state since the last time pressed was called.
		 * Any pin that has changed since the last call to this method will have its bit set.
		 */
		uint8_t changed();
	};
}

#endif
