#include "Encoders.h"

using namespace digitalcave;

ButtonAVR button1(&PORTD, PORTD2, 30, 25, 800, 500);
ButtonAVR button2(&PORTD, PORTD3, 30, 25, 800, 500);
static volatile int8_t encoder1_movement = 0;
static volatile int8_t encoder2_movement = 0;

Encoders::Encoders(){
	PCICR |= _BV(PCIE0);												//Enable pin change interrupts for encoders
	PCMSK0 |= 0x0F;														//Enable bits 0..3 for pin change interrupts
	PORTB |= _BV(PORTB0) | _BV(PORTB1) | _BV(PORTB2) | _BV(PORTB3);		//Pullups on 0..3
}

int8_t Encoders::get_encoder1_movement(){
	double result = encoder1_movement;
	encoder1_movement = 0;
	return result;
}

int8_t Encoders::get_encoder2_movement(){
	double result = encoder2_movement;
	encoder2_movement = 0;
	return result;
}

ISR(PCINT0_vect){
	static uint8_t encoder1 = 0x00;
	static uint8_t encoder2 = 0x00;

	encoder1 = ((encoder1 << 2) | (PINB & 0x03)) & 0x0F;
	encoder2 = ((encoder2 << 2) | ((PINB >> 2) & 0x03)) & 0x0F;

	switch(encoder1){
		case 0x01:
		case 0x07:
		case 0x08:
		case 0x0E:
			encoder1_movement--;	//Counter Clockwise
			break;
		case 0x02:
		case 0x04:
		case 0x0B:
		case 0x0D:
			encoder1_movement++;	//Clockwise
			break;
	}

	switch(encoder2){
		case 0x01:
		case 0x07:
		case 0x08:
		case 0x0E:
			encoder2_movement--;	//Counter Clockwise
			break;
		case 0x02:
		case 0x04:
		case 0x0B:
		case 0x0D:
			encoder2_movement++;	//Clockwise
			break;
	}
}
