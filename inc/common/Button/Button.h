#ifndef BUTTON_H
#define BUTTON_H

#include <avr/io.h>
#include <stdlib.h>

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
		uint8_t state;

	public:
		/*
		 * Set the targets for press, release, longPress, and repeatPress.  Called from sub class' constructors
		 * These values are how many ms must pass before the given event type is fired.  For instance,
		 * if pressCount is 10, we expect to have 10 calls to sample() all read a button press before the press
		 * is reported.  If you know how frequently sample() is called, you can then calculate the total time for
		 * debounce: if sample() is called every 10ms, and pressCount is 10, then you have a 100ms debounce period
		 * for button pressing.
		 */
		void init(uint16_t pressTime, uint16_t releaseTime, uint16_t longPressTime, uint16_t repeatPressTime);

		/*
		 * Samples the pin (by calling read()), and increments counters accordingly.  This method
		 * should be called on a regular interval to ensure accurate debouncing.
		 * Typically this method should be called every 10 to 12 ms for a stabilization period of 80 to 96 ms.
		 *
		 * This function returns zero if the button is currently released (and debounced) and non-zero if the button 
		 * is currently * pressed (and debounced).  Note that this is *not* the same as pressEvent(), releaseEvent(), longPressEvent(), etc
		 * as those return high only once, immediately after the state has changed, whereas this will always return
		 * non-zero as long as the button is still pressed (and debounced).
		 */
		uint8_t sample(uint32_t time);
		
		/*
 		 * Returns non-zero if this button was newly debounced and marked as pressed; zero otherwise.
		 */
		uint8_t pressEvent();
		
		/*
 		 * Returns non-zero if this button was newly debounced and marked as released; zero otherwise.
		 */
		uint8_t releaseEvent();
		
		/*
 		 * Returns non-zero if this button was newly marked as long pressed; zero otherwise.
		 */
		uint8_t longPressEvent();

		/*
 		 * Returns non-zero if this button was newly marked as long pressed; zero otherwise.
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
