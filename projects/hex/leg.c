#include "leg.h"

extern leg_t legs[LEG_COUNT];

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
		leg_set_position_absolute(l, 0, 0, 0);
	}
	
	leg_delay(2000);
}

void leg_delay(uint16_t delay){
	for (uint16_t d = 0; d < delay; d += DELAY_STEP){
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			for (uint8_t j = 0; j < JOINT_COUNT; j++){
				if (fabs(legs[l].current[j] - legs[l].desired[j]) > fabs(legs[l].step[j])) {
					//legs[l].current[j] += legs[l].step[j];		//TODO
					//legs[l].current[j] = legs[l].desired[j];
				}
				else {
					//legs[l].current[j] = legs[l].desired[j];		//TODO
				}
			}
		}
		//servo_update_pwm();
		
		_delay_ms(DELAY_STEP);
	}
}

void leg_set_position_absolute(uint8_t leg, double x, double y, double z){
	legs[leg].current[COXA] = y;
	legs[leg].current[TIBIA] = z;
	servo_set_angle(leg, COXA, legs[leg].current[COXA]);
	servo_set_angle(leg, TIBIA, legs[leg].current[TIBIA]);
}

void leg_set_position_relative(uint8_t leg, double x, double y, double z){
	legs[leg].current[COXA] = legs[leg].current[COXA] + y;
	legs[leg].current[TIBIA] = legs[leg].current[TIBIA] + z;
	servo_set_angle(leg, COXA, legs[leg].current[COXA]);
	servo_set_angle(leg, TIBIA, legs[leg].current[TIBIA]);
}