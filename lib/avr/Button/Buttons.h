#ifndef BUTTONS_H
#define BUTTONS_H

#include <avr/io.h>
#include <stdlib.h>

namespace digitalcave {
	/*
	 * This class can debounce update eight buttons and switches per instance.
	 * The number of samples taken and the press and release periods can be controlled either by using a loop and blocking, or by using a timer and a counter.
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
		 * Samples the pins 10 times at 10 ms intervals.
		 * This is used for general purpose buttons where the press and release periods are both 100 ms, and it is acceptable to block.
 		 * Returns the mask of buttons that are pressed.
		 * Any pin that is pressed will have its bit set.
		 */
		uint8_t poll();
		
		/*
		 * Integrate the pressed and released samples into thee current state.
		 * This is used when the press period is equal to the release period.
 		 * Returns the mask of buttons that are pressed.
		 */
		uint8_t integrate();
		
		/*
		 * Integrate the press samples into the current state.
		 * This is used when the press period differs from the release period.
		 */
		uint8_t integratePress();
		
		/*
		 * Integrate the release samples into the current state.
		 * This is used when the release period differs from the press period.
		 */
		uint8_t integrateRelease();
		
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
