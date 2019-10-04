#include "AlarmClock.h"

using namespace digitalcave;

#ifdef DEBUG
SerialUSB serialUSB;
#endif

int main(){
	display_init();
	encoder_init();
	music_init();

	//Main program loop
	while (1){
		music_poll();
		state_poll();
		display_update();

		delay_ms(50);	//This defines the blink speed
	}

	return 0;
}
