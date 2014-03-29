#define __DELAY_BACKWARD_COMPATIBLE__ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "lib/pwm/pwm.h"

//Front legs
#define FRONT_LEFT		0
#define FL_COXA			0
#define FL_TIBIA		1
#define FRONT_RIGHT		2
#define FR_COXA			2
#define FR_TIBIA		3

//Middle legs
#define MIDDLE_LEFT		4
#define ML_COXA			4
#define ML_TIBIA		5
#define MIDDLE_RIGHT	6
#define MR_COXA			6
#define MR_TIBIA		7

//Rear legs
#define REAR_LEFT		8
#define RL_COXA			8
#define RL_TIBIA		9
#define REAR_RIGHT		10
#define RR_COXA			10
#define RR_TIBIA		11

#define MIN_PHASE	800
#define NEUTRAL	1500
#define MAX_PHASE	2200

#define WAVE_TIBIA_RAISED	600
#define WAVE_COXA_STEP		100

#define RIPPLE_TIBIA_RAISED	600
#define RIPPLE_COXA_STEP	100

#define TRIPOD_TIBIA_RAISED	800
#define TRIPOD_COXA_STEP	400

#define DELAY_SHORT			60
#define DELAY_MEDIUM		100

int16_t leg_neutral_offset[12] = {
	//Front:
	80,	//L_COXA
	-30,		//L_TIBIA
	0,		//R_COXA
	60,	//R_TIBIA
	
	//Middle:
	0,		//L_COXA
	-50,		//L_TIBIA
	0,		//R_COXA
	80,		//R_TIBIA
	
	//Rear
	0,	//L_COXA
	-50,		//L_TIBIA
	0,	//R_COXA
	100		//R_TIBIA
};

int8_t leg_rotation_direction[12] = {
	//Front:
	1,	//L_COXA
	1,	//L_TIBIA
	-1,	//R_COXA
	-1,	//R_TIBIA
	
	//Middle:
	1,	//L_COXA
	1,	//L_TIBIA
	-1,	//R_COXA
	-1,	//R_TIBIA
	
	//Rear
	1,	//L_COXA
	1,	//L_TIBIA
	-1,	//R_COXA
	-1	//R_TIBIA
};

uint16_t leg_position[12];

void update_leg_position(uint16_t delay){
	for (uint8_t i = 0; i < 12; i++){
		pwm_set_phase(i, leg_position[i]);
	}
	_delay_ms(delay);
}

/**
 * Take a wave gait step with the specified leg.  All other legs will move backwards a small amount.  The specified leg should 
 * be the index to the coxa PWM array; the associated femur is implied as coxa + 1.
 */
void wave_gait(uint8_t leg){
	//Lift tibia to absolute position
	leg_position[leg + 1] = NEUTRAL + leg_neutral_offset[leg + 1] + (WAVE_TIBIA_RAISED * leg_rotation_direction[leg + 1]);
	update_leg_position(DELAY_SHORT);
	
	//Move the coxa forward to neutral + 3x STEP...
	leg_position[leg] = NEUTRAL + leg_neutral_offset[leg] + (WAVE_COXA_STEP * leg_rotation_direction[leg]);
	//... while moving the other coxas back by STEP.
	for (uint8_t i = 0; i < 12; i+=2){
		if (i != leg){
			leg_position[i] -= WAVE_COXA_STEP * leg_rotation_direction[i];
		}
	}
	update_leg_position(DELAY_MEDIUM);

	//Drop tibia to neutral position
	leg_position[leg + 1] = NEUTRAL + leg_neutral_offset[leg + 1];
	update_leg_position(DELAY_SHORT);
}

/**
 * Take a ripple gait step with the specified leg.  This is the same as wave gait, but it only affects one side at a time, and 
 * each side is interleaved together.
 */
void ripple_gait(uint8_t leg){
	//Lift tibia to absolute position
	leg_position[leg + 1] = NEUTRAL + leg_neutral_offset[leg + 1] + (RIPPLE_TIBIA_RAISED * leg_rotation_direction[leg + 1]);
	update_leg_position(DELAY_SHORT);
	
	//Move the coxa forward to neutral
	leg_position[leg] = NEUTRAL + leg_neutral_offset[leg];
	//... while moving the other coxas back by STEP.
	for (uint8_t i = 0; i < 12; i+=2){
		if (i != leg){
			leg_position[i] -= RIPPLE_COXA_STEP * leg_rotation_direction[i];
		}
	}
	update_leg_position(DELAY_SHORT);

	//Drop tibia to neutral position
	leg_position[leg + 1] = NEUTRAL + leg_neutral_offset[leg + 1];
	update_leg_position(DELAY_SHORT);
}

void tripod_gait(uint8_t forward1, uint8_t forward2, uint8_t forward3, uint8_t backward1, uint8_t backward2, uint8_t backward3){
	//Move three legs forward (lifting tibias), while moving the others backward
	//Lift three tibias
	leg_position[forward1 + 1] = NEUTRAL + leg_neutral_offset[forward1 + 1] + (TRIPOD_TIBIA_RAISED * leg_rotation_direction[forward1 + 1]);
	leg_position[forward2 + 1] = NEUTRAL + leg_neutral_offset[forward2 + 1] + (TRIPOD_TIBIA_RAISED * leg_rotation_direction[forward2 + 1]);
	leg_position[forward3 + 1] = NEUTRAL + leg_neutral_offset[forward3 + 1] + (TRIPOD_TIBIA_RAISED * leg_rotation_direction[forward3 + 1]);
	update_leg_position(DELAY_MEDIUM);
	
	//Move three legs forward
	leg_position[forward1] = NEUTRAL + leg_neutral_offset[forward1] + (TRIPOD_COXA_STEP * leg_rotation_direction[forward1]);
	leg_position[forward2] = NEUTRAL + leg_neutral_offset[forward2] + (TRIPOD_COXA_STEP * leg_rotation_direction[forward2]);
	leg_position[forward3] = NEUTRAL + leg_neutral_offset[forward3] + (TRIPOD_COXA_STEP * leg_rotation_direction[forward3]);
	
	//Move three legs backward
	leg_position[backward1] = NEUTRAL + leg_neutral_offset[backward1] - (TRIPOD_COXA_STEP * leg_rotation_direction[backward1]);
	leg_position[backward2] = NEUTRAL + leg_neutral_offset[backward2] - (TRIPOD_COXA_STEP * leg_rotation_direction[backward2]);
	leg_position[backward3] = NEUTRAL + leg_neutral_offset[backward3] - (TRIPOD_COXA_STEP * leg_rotation_direction[backward3]);
	update_leg_position(DELAY_SHORT);
	
	//Drop tibias
	leg_position[forward1 + 1] = NEUTRAL + leg_neutral_offset[forward1 + 1];
	leg_position[forward2 + 1] = NEUTRAL + leg_neutral_offset[forward2 + 1];
	leg_position[forward3 + 1] = NEUTRAL + leg_neutral_offset[forward3 + 1];
	update_leg_position(DELAY_MEDIUM);
}

/**
 * Lifts and positions the leg without moving the robot.  Used to initialize legs for a given gait.  Offset is negative for 
 * positions facing forward, and negative for positioned towards the back of the robot.
 */
void lift_and_position_leg(uint8_t leg, int16_t coxa_offset, int16_t tibia_offset){
	leg_position[leg + 1] = NEUTRAL + leg_neutral_offset[leg + 1] + (WAVE_TIBIA_RAISED * leg_rotation_direction[leg + 1]);
	update_leg_position(DELAY_SHORT);
	leg_position[leg] = NEUTRAL + leg_neutral_offset[leg] + (coxa_offset * leg_rotation_direction[leg]);
	update_leg_position(DELAY_SHORT);
	leg_position[leg + 1] = NEUTRAL + leg_neutral_offset[leg + 1] + (tibia_offset * leg_rotation_direction[leg + 1]);
	update_leg_position(DELAY_SHORT);	
}

void wave_gait_init(){
	lift_and_position_leg(REAR_LEFT, WAVE_COXA_STEP * -2, 0);
	lift_and_position_leg(MIDDLE_LEFT, WAVE_COXA_STEP * -1, 0);
	lift_and_position_leg(FRONT_LEFT, WAVE_COXA_STEP * 0, 0);
	lift_and_position_leg(REAR_RIGHT, WAVE_COXA_STEP * 1, 0);
	lift_and_position_leg(MIDDLE_RIGHT, WAVE_COXA_STEP * 2, 0);
	lift_and_position_leg(FRONT_RIGHT, WAVE_COXA_STEP * 3, 0);
}

void ripple_gait_init(){
	lift_and_position_leg(REAR_LEFT, RIPPLE_COXA_STEP * -5, 0);
	lift_and_position_leg(MIDDLE_LEFT, RIPPLE_COXA_STEP * -3, 0);
	lift_and_position_leg(FRONT_LEFT, RIPPLE_COXA_STEP * -1, 0);
	
	lift_and_position_leg(REAR_RIGHT, RIPPLE_COXA_STEP * -2, 0);
	lift_and_position_leg(MIDDLE_RIGHT, RIPPLE_COXA_STEP * 0, 0);
	lift_and_position_leg(FRONT_RIGHT, RIPPLE_COXA_STEP * -4, 0);
}

//Position the legs in a correct starting position for tripod gait
void tripod_gait_init(){
	
}

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
	
	for (uint8_t i = 0; i < 12; i++){
		leg_position[i] = NEUTRAL + leg_neutral_offset[i];
	}
	
	update_leg_position(2000);	
}

void wave_step(){
	wave_gait(REAR_LEFT);
	wave_gait(MIDDLE_LEFT);
	wave_gait(FRONT_LEFT);
	
	wave_gait(REAR_RIGHT);
	wave_gait(MIDDLE_RIGHT);
	wave_gait(FRONT_RIGHT);
}

void ripple_step(){
	ripple_gait(REAR_LEFT);
	ripple_gait(FRONT_RIGHT);
	ripple_gait(MIDDLE_LEFT);
	ripple_gait(REAR_RIGHT);
	ripple_gait(FRONT_LEFT);
	ripple_gait(MIDDLE_RIGHT);
}

void tripod_step(){
	tripod_gait(FRONT_LEFT, MIDDLE_RIGHT, REAR_LEFT, FRONT_RIGHT, MIDDLE_LEFT, REAR_RIGHT);
	tripod_gait(FRONT_RIGHT, MIDDLE_LEFT, REAR_RIGHT, FRONT_LEFT, MIDDLE_RIGHT, REAR_LEFT);	
}

int main (void){
	servo_init();

	//tripod_gait_init();
	//wave_gait_init();
	ripple_gait_init();
	
	//int offset = 0;
	while(1){
		ripple_step();
	}
}
