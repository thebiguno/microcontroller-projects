#ifndef ENCODERS_H
#define ENCODERS_H

#include <avr/interrupt.h>

#define BUTTON_1						_BV(PORTD2)
#define BUTTON_2						_BV(PORTD3)

#define ENCODER1_CLOCKWISE				_BV(1)
#define ENCODER1_COUNTER_CLOCKWISE		_BV(2)
#define ENCODER2_CLOCKWISE				_BV(3)
#define ENCODER2_COUNTER_CLOCKWISE		_BV(4)

namespace digitalcave {

	class Encoders {
		private:

			
		public:
			static volatile uint8_t encoder_movement;
			
			Encoders();
			
			uint8_t get_encoder_movement();
			
			uint8_t get_hold();
			
			uint8_t get_released();
	};
}

#endif