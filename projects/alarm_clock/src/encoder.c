#include "encoder.h"

static volatile int8_t encoder1_movement = 0;
static volatile uint8_t encoder1_state = 0x00;		//Encoder 1 reads pins 4 and 5
static volatile int8_t encoder2_movement = 0;
static volatile uint8_t encoder2_state = 0x00;		//Encoder 2 reads pins 6 and 7

void encoder_init(){
	PCICR |= _BV(PCIE0);														//Enable pin change interrupts for encoders
	PCMSK0 |= _BV(PCINT4) | _BV(PCINT5) | _BV(PCINT6) | _BV(PCINT7);			//Enable bits 4-7 for pin change interrupts
	PORTB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6) | _BV(PORTB7);				//Pullups on 4-7

	encoder1_state = ((encoder1_state << 2) | ((PINB >> 4) & 0x03)) & 0x0F;		//Set initial state (prevents problems on first read)
	encoder2_state = ((encoder2_state << 2) | ((PINB >> 6) & 0x03)) & 0x0F;		//Set initial state (prevents problems on first read)
}

int8_t encoder_get_movement_1(){
	int8_t result = encoder1_movement;
	encoder1_movement = 0;
	return result;
}

int8_t encoder_get_movement_2(){
	int8_t result = encoder2_movement;
	encoder2_movement = 0;
	return result;
}

ISR(PCINT0_vect){
	encoder1_state = ((encoder1_state << 2) | ((PINB >> 4) & 0x03)) & 0x0F;
	encoder2_state = ((encoder2_state << 2) | ((PINB >> 6) & 0x03)) & 0x0F;

	switch(encoder1_state){
		case 0x01:
//		case 0x07:
//		case 0x08:
//		case 0x0E:
			encoder1_movement--;	//Counter Clockwise
			break;
		case 0x02:
//		case 0x04:
//		case 0x0B:
//		case 0x0D:
			encoder1_movement++;	//Clockwise
			break;
	}

	switch(encoder2_state){
		case 0x01:
		// case 0x07:
		// case 0x08:
		// case 0x0E:
			encoder2_movement--;	//Counter Clockwise
			break;
		case 0x02:
		// case 0x04:
		// case 0x0B:
		// case 0x0D:
			encoder2_movement++;	//Clockwise
			break;
	}
}
