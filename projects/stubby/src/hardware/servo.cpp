#include "servo.h"

void servo_init(Leg** legs){
	//Set up the servos using the leg details + status LEDs
	volatile uint8_t *ports[PWM_COUNT];
	uint8_t pins[PWM_COUNT];

	for (uint8_t l = 0; l < LEG_COUNT; l++){
		for (uint8_t j = 0; j < JOINT_COUNT; j++){
			ports[(l * JOINT_COUNT) + j] = legs*[l].getPort(j);
			pins[(l * JOINT_COUNT) + j] = legs*[l].getPin(j);
		}
		
		for (uint8_t j = 0; j < CALIBRATION_COUNT; j++){
			legs*[l].setCalibration(j, eeprom_read_byte((uint8_t*) (l * CALIBRATION_COUNT) + j));
		}
	}
	
	status_init(ports, pins);
	
	pwm_init(ports, pins, PWM_COUNT, 20000);
	status_set_color(0x00, 0x00, 0x00);
	
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		legs*[l].resetPosition();
	}
	pwm_apply_batch();

	//delay_ms(500);
	
	pwm_stop();	
}
