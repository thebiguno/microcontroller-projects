#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "lib/pwm/pwm.h"

//Front legs
#define FL_COXA		0
#define FL_FEMUR	1
#define FL_TIBIA	2
#define FR_COXA		3
#define FR_FEMUR	4
#define FR_TIBIA	5

//Middle legs
#define ML_COXA		6
#define ML_FEMUR	7
#define ML_TIBIA	8
#define MR_COXA		9
#define MR_FEMUR	10
#define MR_TIBIA	11

//Rear legs
#define RL_COXA		12
#define RL_FEMUR	13
#define RL_TIBIA	14
#define RR_COXA		15
#define RR_FEMUR	16
#define RR_TIBIA	17

#define MIN_PHASE	800
#define NEUTRAL	1500
#define MAX_PHASE	2200

#define TIBIA_RAISE	400
#define COXA_FORWARD	300

int16_t leg_neutral_offset[18] = {
	//Front:
	0,	//L_COXA
	0,		//L_FEMUR
	0,		//L_TIBIA
	0,	//R_COXA
	0,		//R_FEMUR
	0,		//R_TIBIA
	
	//Middle:
	0,	//L_COXA
	0,		//L_FEMUR
	0,		//L_TIBIA
	0,		//R_COXA
	0,		//R_FEMUR
	0,		//R_TIBIA
	
	//Rear
	0,	//L_COXA
	0,		//L_FEMUR
	0,	//L_TIBIA
	0,	//R_COXA
	0,		//R_FEMUR
	0		//R_TIBIA
};

int main (void){
	volatile uint8_t *ports[18];
	ports[FL_COXA] = &PORTA;
	ports[FL_FEMUR] = &PORTA;
	ports[FL_TIBIA] = &PORTA;
	ports[FR_COXA] = &PORTA;
	ports[FR_FEMUR] = &PORTA;
	ports[FR_TIBIA] = &PORTA;
	
	ports[ML_COXA] = &PORTA;
	ports[ML_FEMUR] = &PORTB;
	ports[ML_TIBIA] = &PORTB;
	ports[MR_COXA] = &PORTB;
	ports[MR_FEMUR] = &PORTB;
	ports[MR_TIBIA] = &PORTC;
	
	ports[RL_COXA] = &PORTC;
	ports[RL_FEMUR] = &PORTC;
	ports[RL_TIBIA] = &PORTC;
	ports[RR_COXA] = &PORTC;
	ports[RR_FEMUR] = &PORTC;
	ports[RR_TIBIA] = &PORTB;
	
	uint8_t pins[18];
	pins[FL_COXA] = PORTA0;
	pins[FL_FEMUR] = PORTA1;
	pins[FL_TIBIA] = PORTA2;
	pins[FR_COXA] = PORTA3;
	pins[FR_FEMUR] = PORTA4;
	pins[FR_TIBIA] = PORTA5;
	
	pins[ML_COXA] = PORTA6;
	pins[ML_FEMUR] = PORTB0;
	pins[ML_TIBIA] = PORTB1;
	pins[MR_COXA] = PORTB2;
	pins[MR_FEMUR] = PORTB3;
	pins[MR_TIBIA] = PORTC7;
	
	pins[RL_COXA] = PORTC6;
	pins[RL_FEMUR] = PORTC5;
	pins[RL_TIBIA] = PORTC4;
	pins[RR_COXA] = PORTC3;
	pins[RR_FEMUR] = PORTC2;
	pins[RR_TIBIA] = PORTB4;
	
	pwm_init(ports, pins, 18, 20000);
	
	//pwm_set_phase(ML_FEMUR, 1000);
	for (uint8_t i = 0; i < 18; i++){
		pwm_set_phase(i, NEUTRAL + leg_neutral_offset[i]);	//Set to neutral position
	}
	
	_delay_ms(2000);
	
	//int offset = 0;
	while(1){
		pwm_set_phase(RL_TIBIA, NEUTRAL + leg_neutral_offset[RL_TIBIA] - TIBIA_RAISE);
		pwm_set_phase(MR_TIBIA, NEUTRAL + leg_neutral_offset[MR_TIBIA] - TIBIA_RAISE);
		pwm_set_phase(FL_TIBIA, NEUTRAL + leg_neutral_offset[FL_TIBIA] + TIBIA_RAISE);
		_delay_ms(300);
		
		pwm_set_phase(RR_COXA, NEUTRAL + leg_neutral_offset[RR_COXA]);
		pwm_set_phase(ML_COXA, NEUTRAL + leg_neutral_offset[ML_COXA]);
		pwm_set_phase(FR_COXA, NEUTRAL + leg_neutral_offset[FR_COXA]);
		_delay_ms(300);
		
		pwm_set_phase(RL_COXA, NEUTRAL + leg_neutral_offset[RL_COXA] + COXA_FORWARD);
		pwm_set_phase(MR_COXA, NEUTRAL + leg_neutral_offset[MR_COXA] - COXA_FORWARD);
		pwm_set_phase(FL_COXA, NEUTRAL + leg_neutral_offset[FL_COXA] + COXA_FORWARD);
		_delay_ms(300);
		pwm_set_phase(RL_TIBIA, NEUTRAL + leg_neutral_offset[RL_TIBIA]);
		pwm_set_phase(MR_TIBIA, NEUTRAL + leg_neutral_offset[MR_TIBIA]);
		pwm_set_phase(FL_TIBIA, NEUTRAL + leg_neutral_offset[FL_TIBIA]);
		_delay_ms(300);

		pwm_set_phase(RR_TIBIA, NEUTRAL + leg_neutral_offset[RR_TIBIA] + TIBIA_RAISE);
		pwm_set_phase(ML_TIBIA, NEUTRAL + leg_neutral_offset[ML_TIBIA] + TIBIA_RAISE);
		pwm_set_phase(FR_TIBIA, NEUTRAL + leg_neutral_offset[FR_TIBIA] - TIBIA_RAISE);
		_delay_ms(300);

		pwm_set_phase(RL_COXA, NEUTRAL + leg_neutral_offset[RL_COXA]);
		pwm_set_phase(MR_COXA, NEUTRAL + leg_neutral_offset[MR_COXA]);
		pwm_set_phase(FL_COXA, NEUTRAL + leg_neutral_offset[FL_COXA]);
		_delay_ms(300);

		pwm_set_phase(RR_COXA, NEUTRAL + leg_neutral_offset[RR_COXA] - COXA_FORWARD);
		pwm_set_phase(ML_COXA, NEUTRAL + leg_neutral_offset[ML_COXA] + COXA_FORWARD);
		pwm_set_phase(FR_COXA, NEUTRAL + leg_neutral_offset[FR_COXA] - COXA_FORWARD);
		_delay_ms(300);
		pwm_set_phase(RR_TIBIA, NEUTRAL + leg_neutral_offset[RR_TIBIA]);
		pwm_set_phase(ML_TIBIA, NEUTRAL + leg_neutral_offset[ML_TIBIA]);
		pwm_set_phase(FR_TIBIA, NEUTRAL + leg_neutral_offset[FR_TIBIA]);
		_delay_ms(300);
	}
}
