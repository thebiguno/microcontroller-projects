#include "alarmclock.h"

using namespace digitalcave;

int main(){
	state_init();
	display_init();

	DDRF |= _BV(PORTF4);

	//Main program loop
	while (1){
		state_poll();
		display_update();

		PORTF ^= _BV(PORTF4);

		// for (uint8_t i = 0; i < 100; i++){
		//
		// }
		//delay_ms(2);
	}

	return 0;
}
