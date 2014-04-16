#include "stubby.h"

leg_t legs[LEG_COUNT];

int main (void){
	comm_init();
	leg_init();
	
	DDRA |= _BV(PORTA1) | _BV(PORTA4);
	
	for (uint8_t l = 0; l < LEG_COUNT; l++){
		servo_set_angle(l, COXA, 0);
	}
	_delay_ms(2000);
	
	while(1){
		//ripple_step();
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, COXA, 1.5);
		}
		_delay_ms(2000);
		for (uint8_t l = 0; l < LEG_COUNT; l++){
			servo_set_angle(l, COXA, -1.5);
		}
		_delay_ms(2000);
	}
}
