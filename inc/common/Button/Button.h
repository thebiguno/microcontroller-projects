#ifndef BUTTON_H
#define BUTTON_H

#include <stdint.h>
#include <stdlib.h>

#define BUTTON_NOT_PRESSED			0
#define BUTTON_PRESSED 				1
#define BUTTON_LONG_PRESSED			2

namespace digitalcave {
	/*
	 * A fully featured button / debouncing class.  Supports press debounce, release debounce, hold, and repeat.
	 */
	class Button {

	private:
		//The threshold values.  Each state counter must exceed this value to trigger an event.
		uint16_t pressTime;
		uint16_t releaseTime;
		uint16_t longPressTime;
		uint16_t repeatPressTime;

		//State counters.
		uint16_t pressCounter;
		uint16_t releaseCounter;
		uint16_t longPressCounter;
		uint16_t repeatPressCounter;

		//Last system time
		uint32_t lastTime;

		//Event state
		uint8_t eventState;
		//Pressed state
		uint8_t pressedState;

	protected:
		/*
		 * Set the targets for press, release, longPress, and repeatPress.
		 * These values are how many ms must pass before the given event type is fired.
		 */
		Button(uint16_t pressTime, uint16_t releaseTime, uint16_t longPressTime, uint16_t repeatPressTime);


	public:
		/*
		 * Call these static methods, with 2 or more buttons passed in, to determine if all the buttons
		 * are pressed.  This returns true when all but one button is already pressed, and the last button
		 * is then marked as pressed.  You still need to sample() each button manually prior to calling this.
		 */
		static uint8_t allPressEvent(Button* b1, Button* b2);
		static uint8_t allPressEvent(Button* b1, Button* b2, Button* b3);
		static uint8_t allPressEvent(Button* b1, Button* b2, Button* b3, Button* b4);

		/*
		 * Call these static methods, with 2 or more buttons passed in, to determine if all the buttons
		 * are long pressed.  This returns true when all but one button is already pressed, and the last button
		 * is then marked as pressed.  You still need to sample() each button manually prior to calling this.
		 */
		static uint8_t allLongPressEvent(Button* b1, Button* b2);
		static uint8_t allLongPressEvent(Button* b1, Button* b2, Button* b3);
		static uint8_t allLongPressEvent(Button* b1, Button* b2, Button* b3, Button* b4);

		/*
		 * Samples the pin (by calling read()), and increments counters accordingly.  This method
		 * should be called on a regular interval to ensure accurate debouncing.
		 * Typically this method should be called every 10 to 12 ms for a stabilization period of 80 to 96 ms.
		 *
		 * This function returns zero if the button is currently released (and debounced), BUTTON_PRESSED (1) if the button
		 * is currently pressed (and debounced), and BUTTON_LONG_PRESSED (2) if the button is currently longPressed.  Note that this
		 * is *not* the same as pressEvent(), releaseEvent(), longPressEvent(), etc, as those return non-zero
		 * only once, immediately after the state has changed, whereas this will always return non-zero as
		 * long as the button is still pressed (and debounced).
		 */
		uint8_t sample(uint32_t time);

		/*
		 * Returns button state.  BUTTON_NOT_PRESSED (0) is unpressed; BUTTON_PRESSED (1) is pressed; BUTTON_LONG_PRESSED (2) is longPressed.  This will always return the same value
		 * as the last call to sample().
		 */
		uint8_t getState();

		/*
 		 * Returns non-zero if this button was newly debounced and marked as pressed; zero otherwise.
		 */
		uint8_t pressEvent();

		/*
 		 * Returns non-zero if this button was newly debounced and marked as released (and if the timer has not already passed the longPress timeout); zero otherwise.
		 */
		uint8_t releaseEvent();

		/*
 		 * Returns non-zero if this button was newly marked as long pressed; zero otherwise.
		 */
		uint8_t longPressEvent();

		/*
 		 * Returns non-zero if this button was newly released after passing the long pressed threshold; zero otherwise.
		 */
		uint8_t longReleaseEvent();

		/*
 		 * Returns non-zero if this button was newly marked as repeat pressed; zero otherwise.
		 */
		uint8_t repeatPressEvent();

		/*
		 * Interface with the hardware.  This MUST be implemented by subclasses.  This MUST return
		 * 0 if the button is unpressed, and non-zero if the button is pressed.
		 */
		virtual uint8_t read() = 0;
	};
}

#endif
