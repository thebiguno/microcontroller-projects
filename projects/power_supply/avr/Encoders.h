#ifndef ENCODERS_H
#define ENCODERS_H

#include <avr/interrupt.h>

#include "lib/Button/Buttons.h"

#define BUTTON_1						_BV(PORTD2)
#define BUTTON_2						_BV(PORTD3)

#define ENCODER1_CLOCKWISE				_BV(1)
#define ENCODER1_COUNTER_CLOCKWISE		_BV(2)
#define ENCODER2_CLOCKWISE				_BV(3)
#define ENCODER2_COUNTER_CLOCKWISE		_BV(4)

extern "C" void PCINT0_vect()		__attribute__ ((signal));

namespace digitalcave {

	class Encoders {
		private:

		public:
			//Normally these would be instance variables.  However, due to
			// the intersection of ISRs and C++ classes, they have to be
			// class variables.
			//TODO Try to make this private, using 'friend' keyword
			//static volatile uint8_t encoder_movement = 0;
			
			Encoders();
			
			uint8_t get_encoder_movement();
			
			uint8_t get_held();
			
			uint8_t get_released();
			
			friend void PCINT0_vect();
	};
}

#endif