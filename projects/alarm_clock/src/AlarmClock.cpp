#include "AlarmClock.h"

using namespace digitalcave;

#ifdef DEBUG
SerialUSB serialUSB;
#endif

SerialAVR serialAVR(9600, 8, 0, 1, 1);		//Serial Port 1 is the hardware serial port

Sound sound;
State state;

int main(){
	display_init();
	encoder_init();

	//Main program loop
	while (1){
		sound.poll();
		state.poll();
		display_update();

		delay_ms(50);	//This defines the blink speed
	}

	return 0;
}
