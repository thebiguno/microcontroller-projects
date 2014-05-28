#include "stubby.h"

leg_t legs[LEG_COUNT];

int main (void){
	comm_init();
	leg_init();
	status_init();
	
	_delay_ms(1000);
	
	calibration_main();
	
	while(1){
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, TIBIA, 30);
			servo_set_angle(l, FEMUR, -50);
		}
		_delay_ms(1000);
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, TIBIA, -30);
		}
		_delay_ms(10);
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, TIBIA, -50);
			servo_set_angle(l, FEMUR, 50);
		}
		_delay_ms(1000);
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, TIBIA, 0);
			servo_set_angle(l, FEMUR, 0);
		}
		_delay_ms(1000);

		
		
		/*
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, TIBIA, 60);
		}
		_delay_ms(1000);
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, TIBIA, -60);
		}
		_delay_ms(1000);
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, TIBIA, 0);
		}
		_delay_ms(1000);
		
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, FEMUR, 60);
		}
		_delay_ms(1000);
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, FEMUR, -60);
		}
		_delay_ms(1000);
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, FEMUR, 0);
		}
		_delay_ms(1000);

		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, COXA, 60);
		}
		_delay_ms(1000);
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, COXA, -60);
		}
		_delay_ms(1000);
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, COXA, 0);
		}
		_delay_ms(1000);
		*/

	}
}
