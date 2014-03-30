#include "servo.h"

void servo_init(){
	volatile uint8_t *ports[12];
	ports[FL_COXA] = &PORTA;
	ports[FL_TIBIA] = &PORTA;
	ports[FR_COXA] = &PORTA;
	ports[FR_TIBIA] = &PORTA;
	
	ports[ML_COXA] = &PORTA;
	ports[ML_TIBIA] = &PORTB;
	ports[MR_COXA] = &PORTB;
	ports[MR_TIBIA] = &PORTC;
	
	ports[RL_COXA] = &PORTC;
	ports[RL_TIBIA] = &PORTC;
	ports[RR_COXA] = &PORTC;
	ports[RR_TIBIA] = &PORTB;
	
	uint8_t pins[18];
	pins[FL_COXA] = PORTA0;
	pins[FL_TIBIA] = PORTA2;
	pins[FR_COXA] = PORTA3;
	pins[FR_TIBIA] = PORTA5;
	
	pins[ML_COXA] = PORTA6;
	pins[ML_TIBIA] = PORTB1;
	pins[MR_COXA] = PORTB2;
	pins[MR_TIBIA] = PORTC7;
	
	pins[RL_COXA] = PORTC6;
	pins[RL_TIBIA] = PORTC4;
	pins[RR_COXA] = PORTC3;
	pins[RR_TIBIA] = PORTB4;
	
	pwm_init(ports, pins, 12, 20000);
}
