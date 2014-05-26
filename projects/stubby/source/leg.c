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
		double difference = fabs(legs[leg].current[j] - legs[leg].desired[j]);
		legs[leg].step[j] = difference / millis * DELAY_STEP;
		if (legs[leg].step[j] > difference) legs[leg].step[j] = difference;
		else if (legs[leg].step[j] < MIN_STEP) legs[leg].step[j] = MIN_STEP;
	}
}




void leg_init(){
	legs[FRONT_LEFT].port[COXA] = &PORTB;		//PWM00
	legs[FRONT_LEFT].pin[COXA] = PORTB2;
	legs[FRONT_LEFT].port[FEMUR] = &PORTB;		//PWM01
	legs[FRONT_LEFT].pin[FEMUR] = PORTB1;
	legs[FRONT_LEFT].port[TIBIA] = &PORTB;		//PWM02
	legs[FRONT_LEFT].pin[TIBIA] = PORTB0;
	
	legs[FRONT_RIGHT].port[TIBIA] = &PORTA;		//PWM03
	legs[FRONT_RIGHT].pin[TIBIA] = PORTA4;
	legs[FRONT_RIGHT].port[FEMUR] = &PORTA;		//PWM04
	legs[FRONT_RIGHT].pin[FEMUR] = PORTA5;
	legs[FRONT_RIGHT].port[COXA] = &PORTA;		//PWM05
	legs[FRONT_RIGHT].pin[COXA] = PORTA6;

	legs[MIDDLE_LEFT].port[COXA] = &PORTB;		//PWM06
	legs[MIDDLE_LEFT].pin[COXA] = PORTB4;
	legs[MIDDLE_LEFT].port[FEMUR] = &PORTA;		//PWM07
	legs[MIDDLE_LEFT].pin[FEMUR] = PORTA3;
	legs[MIDDLE_LEFT].port[TIBIA] = &PORTB;		//PWM08
	legs[MIDDLE_LEFT].pin[TIBIA] = PORTB3;

	legs[MIDDLE_RIGHT].port[TIBIA] = &PORTC;	//PWM09
	legs[MIDDLE_RIGHT].pin[TIBIA] = PORTC7;
	legs[MIDDLE_RIGHT].port[FEMUR] = &PORTC;	//PWM10
	legs[MIDDLE_RIGHT].pin[FEMUR] = PORTC6;
	legs[MIDDLE_RIGHT].port[COXA] = &PORTC;		//PWM11
	legs[MIDDLE_RIGHT].pin[COXA] = PORTC5;

	legs[REAR_LEFT].port[COXA] = &PORTC;		//PWM12
	legs[REAR_LEFT].pin[COXA] = PORTC4;
	legs[REAR_LEFT].port[FEMUR] = &PORTD;		//PWM13
	legs[REAR_LEFT].pin[FEMUR] = PORTD3;
	legs[REAR_LEFT].port[TIBIA] = &PORTD;		//PWM14
	legs[REAR_LEFT].pin[TIBIA] = PORTD2;

	legs[REAR_RIGHT].port[TIBIA] = &PORTC;		//PWM15
	legs[REAR_RIGHT].pin[TIBIA] = PORTC3;
	legs[REAR_RIGHT].port[FEMUR] = &PORTC;		//PWM16
	legs[REAR_RIGHT].pin[FEMUR] = PORTC2;
	legs[REAR_RIGHT].port[COXA] = &PORTD;		//PWM17
	legs[REAR_RIGHT].pin[COXA] = PORTD7;
	
	servo_init();
	
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		for (uint8_t j = 0; j < JOINT_COUNT; j++){
			legs[l].step[j] = MIN_STEP;
		}
		leg_set_current_position_absolute(l, 0, 0, TIBIA_LOWERED);
	}
	
	servo_apply_batch();
	
	leg_delay_ms(100);
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
		
		servo_apply_batch();
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