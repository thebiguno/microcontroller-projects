#include "hex.h"

leg_t legs[LEG_COUNT];

int main (void){
	comm_init();
	leg_init();
	
	DDRA |= _BV(PORTA1);
	
	while(1){
		ripple_step();
	}
}
