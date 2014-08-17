#ifndef BUTTONS_H
#define BUTTONS_H

#include <avr/io.h>
#include <stdlib.h>

namespace digitalcave {
	/*
	 * This class can debounce update eight buttons and switches per instance.
	 * Debouning is performed with a sliding window of size 8 so the sample frequency = stabilization period / 8.
	 * General purpose buttons have press and release periods from 50 to 100 ms.
	 * Repeate action buttons have press and release periods from 20 to 40 ms (to increase the repeat rate).
	 * Reset buttons have press periods from 250 to 500 ms, and release periods from 50 to 100 ms (to prevent accidental presses).
	 * Toggle buttons have press periods from 50 to 100 ms, and release periods from 500 to 1000 ms (to prevent double toggling).
	 * Since all the buttons share press and release state they must all have the same characteristics.  If this is not desirable a second instance could be used to have different sample rates and press/release periods.
	 */
	class Buttons {

	private:
		volatile uint8_t *port;
		volatile uint8_t *pin;
		uint8_t pins_bv;
		uint8_t current;
		uint8_t last;
		uint8_t pressed_bv;
		uint8_t released_bv;
		uint8_t window[8];

	public:
		/*
		 * Creates a new Buttons class that can debounce the selected pins of a given port.
		 * The selected pins are configured for input and pull ups are enabled making this library suitable only for active-low configuration.
		 * The number of pressed and released samples to consider when changing state can be controlled with pressed and released which take values from 1 to 8.
		 * For a general purpose button with a press/release period of 100 ms and uniform sampling, set pressed and released to 1 and call sample every 100 ms.
		 * For a general purpose button with a press/release period of 100 ms and windowed sampling, set pressed and released to 8 and call sample every 12 ms.
		 * For a toggle buttons with a press period of 100 ms and a release period of 400 ms, set pressed to 2 and released to 8 and call sample every 50 ms.
		 * For a reset button with a press period of 800 ms and a release period af 100 ms, set pressed to 8 and released to 1 and call sample every 100 ms.
		 */
		Buttons(volatile uint8_t *port, uint8_t pins, uint8_t pressed, uint8_t released);
		
		/*
		 * Samples the pins once.
		 * This method should be called as frequently as 3 milliseconds for a stabilization period of 20 ms,
		 * or as infrequently as 60 ms for a stabilization period of 480 ms.
		 * Typically this method should be called every 10 to 12 ms for a stabilization period of 80 to 96 ms.
		 */
		uint8_t sample();
		
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
