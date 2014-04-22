#include "stubby.h"

leg_t legs[LEG_COUNT];

static double velocity;
static double direction;
static uint8_t special;
static uint8_t last_special;

int main (void){
	comm_init();
	leg_init();
	
	DDRA |= _BV(PORTA1) | _BV(PORTA4);
/*	
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		servo_set_angle(l, COXA, 0);
	}
	_delay_ms(2000);
*/	
	while(1){
		comm_read(&velocity, &direction, &special);
		
		if ((special & SPECIAL_RESET) && !(last_special & SPECIAL_RESET)){
			gait_init();
			last_special |= SPECIAL_RESET;
		}
		else if (velocity < -0.1 || velocity > 0.1 || direction < -0.1 || direction > 0.1) {
			gait_step(velocity, direction);
			last_special &= ~SPECIAL_RESET;
		}
		else {
			leg_delay_ms(20);
		}
		
		/*
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, COXA, 1.5);
			servo_set_angle(l, TIBIA, 1.5);
		}
		_delay_ms(2000);
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, COXA, -1.5);
			servo_set_angle(l, TIBIA, -1.5);
		}
		_delay_ms(2000);
		*/
	}
}
