#include "alarmclock.h"

using namespace digitalcave;

int main(){
	state_init();
	display_init();

	//Main program loop
	while (1){
		state_poll();
		display_update();

		delay_ms(50);	//This defines the blink speed
	}

	return 0;
}
