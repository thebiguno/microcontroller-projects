#include "leg.h"

extern leg_t *legs;

void leg_init(){
	legs[FRONT_LEFT].port[COXA] = &PORTA;
	legs[FRONT_LEFT].pin[COXA] = PORTA0;
	legs[FRONT_LEFT].port[TIBIA] = &PORTA;
	legs[FRONT_LEFT].pin[TIBIA] = PORTA2;
	
	legs[FRONT_RIGHT].port[COXA] = &PORTA;
	legs[FRONT_RIGHT].pin[COXA] = PORTA3;
	legs[FRONT_RIGHT].port[TIBIA] = &PORTA;
	legs[FRONT_RIGHT].pin[TIBIA] = PORTA5;

	legs[MIDDLE_LEFT].port[COXA] = &PORTA;
	legs[MIDDLE_LEFT].pin[COXA] = PORTA6;
	legs[MIDDLE_LEFT].port[TIBIA] = &PORTB;
	legs[MIDDLE_LEFT].pin[TIBIA] = PORTB1;

	legs[MIDDLE_RIGHT].port[COXA] = &PORTB;
	legs[MIDDLE_RIGHT].pin[COXA] = PORTB2;
	legs[MIDDLE_RIGHT].port[TIBIA] = &PORTC;
	legs[MIDDLE_RIGHT].pin[TIBIA] = PORTC7;

	legs[REAR_LEFT].port[COXA] = &PORTC;
	legs[REAR_LEFT].pin[COXA] = PORTC6;
	legs[REAR_LEFT].port[TIBIA] = &PORTC;
	legs[REAR_LEFT].pin[TIBIA] = PORTC4;

	legs[REAR_RIGHT].port[COXA] = &PORTC;
	legs[REAR_RIGHT].pin[COXA] = PORTC3;
	legs[REAR_RIGHT].port[TIBIA] = &PORTB;
	legs[REAR_RIGHT].pin[TIBIA] = PORTB4;
	
	servo_init();
	
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		leg_set_position(l, 0, 0, 0, 1);
	}
	
	leg_delay(2000);
}

void leg_delay(uint16_t delay){
	for (uint16_t d = 0; d < delay; d += DELAY_STEP){
		for (uint8_t l = 0; l < 6; l++){
			for (uint8_t j = 0; j < 3; j++){
				if (fabs(legs[l].current[j] - legs[l].desired[j]) > fabs(legs[l].step[j])) {
					legs[l].current[j] += legs[l].step[j];
				}
				else {
					legs[l].current[j] = legs[l].desired[j];
				}
			}
		}
		servo_update_pwm();
		
		_delay_ms(DELAY_STEP);
	}
}

void leg_set_position(uint8_t leg, double x, double y, double z, uint16_t time){
	servo_set_angle(leg, COXA, y, time);
	servo_set_angle(leg, TIBIA, z, time);
}

/*
uint16_t leg_position[12];

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

void update_leg_position(uint16_t delay){
	for (uint8_t i = 0; i < 12; i++){
		pwm_set_phase(i, leg_position[i]);
	}
	_delay_ms(delay);
}

void lift_and_position_leg(uint8_t leg, int16_t coxa_offset, int16_t tibia_offset){
	leg_position[leg + 1] = NEUTRAL + leg_neutral_offset[leg + 1] + (TIBIA_RAISED * leg_rotation_direction[leg + 1]);
	update_leg_position(DELAY_SHORT);
	leg_position[leg] = NEUTRAL + leg_neutral_offset[leg] + (coxa_offset * leg_rotation_direction[leg]);
	update_leg_position(DELAY_SHORT);
	leg_position[leg + 1] = NEUTRAL + leg_neutral_offset[leg + 1] + (tibia_offset * leg_rotation_direction[leg + 1]);
	update_leg_position(DELAY_SHORT);	
}

void leg_init(){
	for (uint8_t i = 0; i < 12; i++){
		leg_position[i] = NEUTRAL + leg_neutral_offset[i];
	}
	
	update_leg_position(2000);
}
*/