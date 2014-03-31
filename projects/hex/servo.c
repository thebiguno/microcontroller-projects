#include "servo.h"

extern leg_t *legs;

void servo_init(){
	volatile uint8_t *ports[12];
	uint8_t pins[12];

	for (uint8_t l = 0; l < 6; l++){
		ports[l * 3] = legs[l].port[COXA];
		pins[l * 3] = legs[l].pin[COXA];
		ports[(l * 3) + 1] = legs[l].port[TIBIA];
		pins[(l * 3) + 1] = legs[l].pin[TIBIA];
		
		legs[l].direction[COXA] = (l & 0x01) ? -1 : 1;
		legs[l].direction[TIBIA] = (l & 0x01) ? -1 : 1;
	}
	
	//TODO Allow for calibration via controller, plus store to EEPROM
	legs[FRONT_LEFT].offset[COXA] = 80;
	legs[FRONT_LEFT].offset[TIBIA] = -30;
	legs[FRONT_RIGHT].offset[COXA] = 0;
	legs[FRONT_LEFT].offset[TIBIA] = 60;
	
	legs[MIDDLE_LEFT].offset[COXA] = 0;
	legs[MIDDLE_LEFT].offset[TIBIA] = -50;
	legs[MIDDLE_RIGHT].offset[COXA] = 0;
	legs[MIDDLE_RIGHT].offset[TIBIA] = 80;
	
	legs[REAR_LEFT].offset[COXA] = 0;
	legs[REAR_LEFT].offset[TIBIA] = -50;
	legs[REAR_RIGHT].offset[COXA] = 0;
	legs[REAR_RIGHT].offset[TIBIA] = 100;
	
	pwm_init(ports, pins, 12, 20000);
}

void servo_set_angle(uint8_t leg, uint8_t joint, double angle, uint16_t time){
	if (time == 0) time = 1;
	
	legs[leg].desired[joint] = NEUTRAL + (legs[leg].offset[joint] * legs[leg].direction[joint]) + (angle * ((MAX_PHASE - MIN_PHASE) / SERVO_TRAVEL) * legs[leg].direction[joint]);
	legs[leg].step[joint] = (legs[leg].current[joint] - legs[leg].desired[joint]) / time;
}

void servo_update_pwm(){
	for (uint8_t l = 0; l < 6; l++){
		pwm_set_phase(l * 2, legs[l].current[COXA]);
		pwm_set_phase((l * 2) + 1, legs[l].current[TIBIA]);
	}
}