#ifndef ENCODER_H
#define ENCODER_H

#include <avr/interrupt.h>

#include <Button/ButtonAVR.h>

extern "C" void PCINT0_vect()		__attribute__ ((signal));

namespace digitalcave {

	class Encoder {
		private:

		public:
			//Normally these would be instance variables.  However, due to
			// the intersection of ISRs and C++ classes, they have to be
			// class variables.

			Encoder();

			int8_t get_lamp_encoder_movement();
			int8_t get_music_encoder_movement();

			friend void PCINT0_vect();
	};
}

#endif
