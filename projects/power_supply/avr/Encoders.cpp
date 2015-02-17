#include "Encoders.h"

using namespace digitalcave;

Encoders::Encoders(){
	//Enable pin change interrupts for encoders
	PCICR |= _BV(PCIE0);
	PCMSK0 |= 0x0F;	//Enable bits 0..3 for pin change interrupts
	PORTB |= _BV(PORTB0) | _BV(PORTB1) | _BV(PORTB2) | _BV(PORTB3);
	sei();
}

uint8_t Encoders::get_encoder_movement(){
	return Encoders::encoder_movement;
}

uint8_t Encoders::get_hold(){
	return 0;	//TODO
}

uint8_t Encoders::get_released(){
	return 0;	//TODO
}

ISR(PCINT0_vect){
	static uint8_t encoder1 = 0x00;
	static uint8_t encoder2 = 0x00;
	
	encoder1 = ((encoder1 << 2) | (PINB & 0x03)) & 0x0F;
	encoder2 = ((encoder2 << 2) | ((PINB >> 2) & 0x03)) & 0x0F;
	
	//The encoders I am using now output a full sequence for one step, so we only look for one
	// of the sub-sequences to avoid 4x faster movement than we want.  Change this as needed.
	switch(encoder1){
		case 0x01:
		//case 0x07:
		//case 0x08:
		//case 0x0E:
			Encoders::encoder_movement = ENCODER1_CLOCKWISE;
			break;
		case 0x02:
		//case 0x04:
		//case 0x0B:
		//case 0x0D:
			Encoders::encoder_movement = ENCODER1_COUNTER_CLOCKWISE;
			break;
	}
	
	switch(encoder2){
		case 0x01:
		//case 0x07:
		//case 0x08:
		//case 0x0E:
			Encoders::encoder_movement = ENCODER2_CLOCKWISE;
			break;
		case 0x02:
		//case 0x04:
		//case 0x0B:
		//case 0x0D:
			Encoders::encoder_movement = ENCODER2_COUNTER_CLOCKWISE;
			break;
	}
}