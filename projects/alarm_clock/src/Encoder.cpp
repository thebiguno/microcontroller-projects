#include "Encoder.h"

using namespace digitalcave;

static volatile int8_t lamp_encoder_movement = 0;
static volatile uint8_t lamp_encoder_state = 0x00;		//Lamp encoder reads pins 4 and 6
static volatile int8_t music_encoder_movement = 0;
static volatile uint8_t music_encoder_state = 0x00;		//Music encoder reads pins 5 and 6

Encoder::Encoder(){
	PCICR |= _BV(PCIE0);														//Enable pin change interrupts for encoders
	PCMSK0 |= _BV(PCINT4) | _BV(PCINT5) | _BV(PCINT6) | _BV(PCINT7);			//Enable bits 4-7 for pin change interrupts
	PORTB |= _BV(PORTB4) | _BV(PORTB5) | _BV(PORTB6) | _BV(PORTB7);				//Pullups on 4-7

	lamp_encoder_state = ((lamp_encoder_state << 2) | ((PINB >> 4) & 0x03)) & 0x0F;			//Set initial state (prevents problems on first read)
	music_encoder_state = ((music_encoder_state << 2) | ((PINB >> 6) & 0x03)) & 0x0F;		//Set initial state (prevents problems on first read)
}

int8_t Encoder::get_lamp_encoder_movement(){
	int8_t result = lamp_encoder_movement;
	lamp_encoder_movement = 0;
	return result >> 2;		//divide by 4 since these encoders give 4 pulses per detent
}

int8_t Encoder::get_music_encoder_movement(){
	int8_t result = music_encoder_movement;
	music_encoder_movement = 0;
	return result >> 2;		//divide by 4 since these encoders give 4 pulses per detent
}

ISR(PCINT0_vect){
	lamp_encoder_state = ((lamp_encoder_state << 2) | ((PINB >> 4) & 0x03)) & 0x0F;
	music_encoder_state = ((music_encoder_state << 2) | ((PINB >> 6) & 0x03)) & 0x0F;

	switch(lamp_encoder_state){
		case 0x01:
		case 0x07:
		case 0x08:
		case 0x0E:
			lamp_encoder_movement--;	//Counter Clockwise
			break;
		case 0x02:
		case 0x04:
		case 0x0B:
		case 0x0D:
			lamp_encoder_movement++;	//Clockwise
			break;
	}

	switch(music_encoder_state){
		case 0x01:
		case 0x07:
		case 0x08:
		case 0x0E:
			music_encoder_movement--;	//Counter Clockwise
			break;
		case 0x02:
		case 0x04:
		case 0x0B:
		case 0x0D:
			music_encoder_movement++;	//Clockwise
			break;
	}
}
