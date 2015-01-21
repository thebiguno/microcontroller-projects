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
		
		uint8_t _current;
		uint8_t _last;
		uint8_t _held;
		uint8_t _repeat;
		
		uint8_t pressed_bv;
		uint8_t released_bv;
		uint8_t held_time;
		uint8_t repeat_time;
		uint8_t held_timer[8];
		uint8_t repeat_timer[8];
		uint8_t window[8];

	public:
		/*
		 * Creates a new Buttons class that can debounce the selected pins of a given port.
		 * The selected pins are configured for input and pull ups are enabled making this library suitable only 
		 * for active-low configuration.
		 *
		 * The extra arguments (press_time, release_time, hold_time, and repeat_interval) indicate the count of 
		 * how many sample iterations must go by before each of these states is met, and are defined as below:
		 * press_time:   The number of iterations of sample() which must see the button as pressed before it is reported 
		 *               as being pressed.  This merges the concerns of debouncing with the UI concern of how 'hard' a 
		 *               button must be pressed (see below for UI guidelines).  This must be a number between 1 and 8.
		 *               When calling pressed() function, a button that is pressed will only appear once (the first time
		 *               its state changes from non-pressed to pressed.
		 * release_time: The same as press time, but the number of iterations before the button is considered to be released.
		 *               As with press time, this must be a number between 1 and 8.
		 * hold_time:    The number of sample() calls which must pass before the button is considered to be held.  The held
		 *               flag will appear for the pin in question when the held() function is called.  hold_time is an 8 bit
		 *               number between 0 and 255.  Set hold_time to 0 to disable hold functionality.
		 * repeat_time:  The number of sample() calls which must pass after hold_time and after the last repeat event before
		 *               another repeat flag is set.  This is an 8 bit number between 0 and 255.  Set repeat_time to 0 to 
		 *               disable repeat functionality.
		 *
		 * You MUST call sample() repeatedly at the specified period (12ms is a good place to start).  The return of sample()
		 * will show the currently pressed buttons.  If you want more state information, you then also MUST
		 * call pressed(), released(), held(), and repeat() (or at least the subset that you are interested in) 
		 * once for every time that you call sample(), or else events may be missed.
		 * 
		 * User Interface Considerations:
		 * For general purpose buttons with a press/release period of 100 ms and windowed sampling, set pressed and 
		 *  released to 8 and call sample every 12 ms.
		 * For toggle buttons with a press period of 100 ms and a release period of 400 ms, set pressed to 2 and 
		 *  released to 8 and call sample every 50 ms.
		 * For reset buttons with a press period of 800 ms and a release period af 100 ms, set pressed to 8 and released
		 *  to 1 and call sample every 100 ms.
		 *
		 * For case #1 (GP button, 100ms press / release), if you wanted a hold period of 3 seconds set hold_time to 250 
		 *  (since 3000 ms divided by 12 ms sample period is 250).  If you wanted the repeat to trigger every 250ms after
		 *  the initial hold was registered, set the repeat_time to 21.
		 */
		Buttons(volatile uint8_t *port, uint8_t pin_mask, uint8_t press_time, uint8_t release_time, uint8_t hold_time, uint8_t repeat_time);

		/*
		 * Convenience method.  This assumes a sample period of 12ms, with press_time and release_time both set to 8 
		 * for a 100ms press / release period, suitable for general purpose buttons.  Hold and repeat are set to 0, 
		 * disabling this functionality.
		 */
		Buttons(volatile uint8_t *port, uint8_t pin_mask);
		
		/*
		 * Samples the pins once.
		 * This method should be called as frequently as 3 milliseconds for a stabilization period of 20 ms,
		 * or as infrequently as 60 ms for a stabilization period of 480 ms.
		 * Typically this method should be called every 10 to 12 ms for a stabilization period of 80 to 96 ms.
		 *
		 * This function returns the currently pressed buttons (*not* the newly pressed buttons; it will mark
		 * a button as being pressed even if it is being held down).
		 */
		uint8_t sample();
		
		/*
 		 * Returns the mask of buttons that are newly pressed.
		 */
		uint8_t pressed();
		
		/*
		 * Returns the mask of buttons that are newly released.
		 */
		uint8_t released();
		
		/*
		 * Returns the mask of buttons that are newly pressed and held.
		 */
		uint8_t held();
		
		/*
		 * Returns the mask of buttons that are firing 'repeat' during this sample iteration.
		 */
		uint8_t repeat();
	};
}

#endif
