#include "servo.h"

extern leg_t legs[LEG_COUNT];

void servo_init(){
	volatile uint8_t *ports[LEG_COUNT * JOINT_COUNT + 3];
	uint8_t pins[LEG_COUNT * JOINT_COUNT + 3];

	for (uint8_t l = 0; l < LEG_COUNT; l++){
		for (uint8_t j = 0; j < JOINT_COUNT; j++){
			ports[(l * JOINT_COUNT) + j] = legs[l].port[j];
			pins[(l * JOINT_COUNT) + j] = legs[l].pin[j];

			legs[l].direction[j] = (l & 0x01) ? -1 : 1;
		}
	}
	
	ports[LED_RED] = &PORTD;		//Red LED
	pins[LED_RED] = PORTD4;
	ports[LED_GREEN] = &PORTD;		//Green LED
	pins[LED_GREEN] = PORTD6;
	ports[LED_BLUE] = &PORTD;		//Blue LED
	pins[LED_BLUE] = PORTD5;
	
	//TODO Allow for calibration via controller, plus store to EEPROM
/*	legs[FRONT_LEFT].offset[COXA] = 80;
	legs[FRONT_LEFT].offset[TIBIA] = -130;
	legs[FRONT_RIGHT].offset[COXA] = 0;
	legs[FRONT_RIGHT].offset[TIBIA] = -210;
	
	legs[MIDDLE_LEFT].offset[COXA] = 0;
	legs[MIDDLE_LEFT].offset[TIBIA] = -180;
	legs[MIDDLE_RIGHT].offset[COXA] = 0;
	legs[MIDDLE_RIGHT].offset[TIBIA] = -230;
	
	legs[REAR_LEFT].offset[COXA] = 0;
	legs[REAR_LEFT].offset[TIBIA] = -200;
	legs[REAR_RIGHT].offset[COXA] = 0;
	legs[REAR_RIGHT].offset[TIBIA] = -300;
*/	
	pwm_init(ports, pins, LEG_COUNT * JOINT_COUNT, 20000);
}

void servo_set_angle(uint8_t leg, uint8_t joint, double angle){
	//Convert from angle to microseconds
	//pwm_set_phase((leg * JOINT_COUNT) + joint, NEUTRAL + (legs[leg].offset[joint] * legs[leg].direction[joint]) + (angle * ((MAX_PHASE - MIN_PHASE) / SERVO_TRAVEL) * legs[leg].direction[joint]));
	pwm_set_phase_batch((leg * JOINT_COUNT) + joint, NEUTRAL + (angle * ((MAX_PHASE - MIN_PHASE) / SERVO_TRAVEL) * legs[leg].direction[joint]));
}
