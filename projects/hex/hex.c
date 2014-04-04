#include "hex.h"

leg_t legs[LEG_COUNT];

int main (void){
	leg_init();
	
	DDRA |= _BV(PORTA1);
	
	ripple_gait_init();
	
	while(1){
		ripple_step();
	}
}
