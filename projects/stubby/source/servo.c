#include "servo.h"

extern Leg legs[LEG_COUNT];

void servo_init(){
	volatile uint8_t *ports[(LEG_COUNT * JOINT_COUNT) + 3];
	uint8_t pins[(LEG_COUNT * JOINT_COUNT) + 3];

	for (uint8_t l = 0; l < LEG_COUNT; l++){
		for (uint8_t j = 0; j < JOINT_COUNT; j++){
			ports[(l * JOINT_COUNT) + j] = legs[l].port[j];
			pins[(l * JOINT_COUNT) + j] = legs[l].pin[j];

			//legs[l].direction[j] = (l & 0x01) ? -1 : 1;
		}
	}
	
	ports[LED_RED] = &PORTD;		//Red LED
	pins[LED_RED] = PORTD4;
	ports[LED_GREEN] = &PORTD;		//Green LED
	pins[LED_GREEN] = PORTD6;
	ports[LED_BLUE] = &PORTD;		//Blue LED
	pins[LED_BLUE] = PORTD5;
	
	servo_load_calibration();
	
	pwm_init(ports, pins, LEG_COUNT * JOINT_COUNT + 3, 20000);
}

inline uint8_t* get_calibration_address(uint8_t l, uint8_t j){
	//Leg calibration data is stored in the EEPROM, starting at address 0x00
	return (uint8_t*) (l * JOINT_COUNT) + j;
}

void servo_load_calibration(){
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		for (uint8_t j = 0; j < JOINT_COUNT; j++){
			legs[l].offset[j] =  eeprom_read_byte(get_calibration_address(l, j));
		}
	}
}

void servo_save_calibration(){
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		for (uint8_t j = 0; j < JOINT_COUNT; j++){
			eeprom_update_byte(get_calibration_address(l, j), legs[l].offset[j]);
		}
	}
}

void servo_set_angle(Leg leg, int8_t angle){
	//Convert from angle to microseconds
	pwm_set_phase_batch((leg * JOINT_COUNT) + , NEUTRAL + ((angle + offset) * ((MAX_PHASE - MIN_PHASE) / SERVO_TRAVEL)));
}
