#include "Encoder.h"

using namespace digitalcave;

static volatile int8_t encoder_movement = 0;

Encoder::Encoder(){
	PCICR |= _BV(PCIE0);												//Enable pin change interrupts for encoders
	PCMSK0 |= _BV(PCINT5) | _BV(PCINT6);								//Enable bits 5 and 6 for pin change interrupts
	PORTB |= _BV(PORTB5) | _BV(PORTB6);									//Pullups on 5 and 6
}

int8_t Encoder::get_encoder_movement(){
	double result = encoder_movement;
	encoder_movement = 0;
	return result;
}

ISR(PCINT0_vect){
	static uint8_t encoder = 0x00;

	encoder = ((encoder << 2) | ((PINB >> 5) & 0x03)) & 0x0F;

	switch(encoder){
		case 0x01:
		//case 0x07:
		//case 0x08:
		//case 0x0E:
			encoder_movement--;	//Counter Clockwise
			break;
		//case 0x02:
		//case 0x04:
		case 0x0B:
		//case 0x0D:
			encoder_movement++;	//Clockwise
			break;
	}
}
