#include "leg.h"

extern leg_t legs[LEG_COUNT];

static inline void bounds_check_current(uint8_t leg){
	if (legs[leg].current[COXA] < -MAX_ANGLE) legs[leg].current[COXA] = -MAX_ANGLE; 
	else if (legs[leg].current[COXA] > MAX_ANGLE) legs[leg].current[COXA] = MAX_ANGLE;
	if (legs[leg].current[TIBIA] < -MAX_ANGLE) legs[leg].current[TIBIA] = -MAX_ANGLE;
	else if (legs[leg].current[TIBIA] > MAX_ANGLE) legs[leg].current[TIBIA] = MAX_ANGLE;
}

static inline void bounds_check_desired(uint8_t leg){
	if (legs[leg].desired[COXA] < -MAX_ANGLE) legs[leg].desired[COXA] = -MAX_ANGLE;
	else if (legs[leg].desired[COXA] > MAX_ANGLE) legs[leg].desired[COXA] = MAX_ANGLE;
	if (legs[leg].desired[TIBIA] < -MAX_ANGLE) legs[leg].desired[TIBIA] = -MAX_ANGLE;
	else if (legs[leg].desired[TIBIA] > MAX_ANGLE) legs[leg].desired[TIBIA] = MAX_ANGLE;
}

static inline void update_current_position(uint8_t leg){
	//By setting current directly, we implicitly also set desired.
	for (uint8_t j = 0; j < JOINT_COUNT; j++){
		legs[leg].desired[j] = legs[leg].current[j];
		servo_set_angle(leg, j, legs[leg].current[j]);
	}
}

static inline void update_desired_position(uint8_t leg, uint16_t millis){
	if (millis == 0) millis = 1;
	for (uint8_t j = 0; j < JOINT_COUNT; j++){
		legs[leg].step[j] = fabs(legs[leg].current[j] - legs[leg].desired[j]) / millis * DELAY_STEP;
		if (legs[leg].step[j] < MIN_STEP) legs[leg].step[j] = MIN_STEP;
	}
}




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
		for (uint8_t j = 0; j < JOINT_COUNT; j++){
			legs[l].step[j] = MIN_STEP;
		}
		leg_set_current_position_absolute(l, 0, 0, TIBIA_LOWERED);
	}
	
	leg_delay_ms(2000);
}

void leg_delay_ms(uint16_t millis){
	for (uint16_t d = 0; d < millis; d += DELAY_STEP){
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			for (uint8_t j = 0; j < JOINT_COUNT; j++){
				double error = legs[l].current[j] - legs[l].desired[j];
				if (error > legs[l].step[j]) {
					legs[l].current[j] = legs[l].current[j] - legs[l].step[j];
					servo_set_angle(l, j, legs[l].current[j]);
				}
				else if (error < (legs[l].step[j] * -1)){
					legs[l].current[j] = legs[l].current[j] + legs[l].step[j];
					servo_set_angle(l, j, legs[l].current[j]);
				}
				else if (error > MIN_STEP) {
					legs[l].current[j] = legs[l].current[j] - MIN_STEP;
					servo_set_angle(l, j, legs[l].current[j]);
				}
				else if (error < (MIN_STEP * -1)){
					legs[l].current[j] = legs[l].current[j] + MIN_STEP;
					servo_set_angle(l, j, legs[l].current[j]);
				}
				else {
					legs[l].current[j] = legs[l].desired[j];
				}
			}
		}
		
		_delay_ms(DELAY_STEP);
	}
}

void leg_set_current_position_absolute(uint8_t leg, double x, double y, double z){
	legs[leg].current[COXA] = y;
	legs[leg].current[TIBIA] = z;
	
	bounds_check_current(leg);
	
	update_current_position(leg);
}

void leg_set_current_position_relative(uint8_t leg, double x, double y, double z){
	legs[leg].current[COXA] = legs[leg].current[COXA] + y;
	legs[leg].current[TIBIA] = legs[leg].current[TIBIA] + z;

	bounds_check_current(leg);

	update_current_position(leg);
}

void leg_set_desired_position_absolute(uint8_t leg, double x, double y, double z, uint16_t millis){
	legs[leg].desired[COXA] = y;
	legs[leg].desired[TIBIA] = z;

	bounds_check_desired(leg);
	
	update_desired_position(leg, millis);
}

void leg_set_desired_position_relative(uint8_t leg, double x, double y, double z, uint16_t millis){
	legs[leg].desired[COXA] = legs[leg].desired[COXA] + y;
	legs[leg].desired[TIBIA] = legs[leg].desired[TIBIA] + z;
	
	bounds_check_desired(leg);
	
	update_desired_position(leg, millis);
}