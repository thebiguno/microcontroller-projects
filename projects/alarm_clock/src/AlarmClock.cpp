#include "AlarmClock.h"

using namespace digitalcave;

#ifdef DEBUG
SerialUSB serialUSB;
#endif

SerialAVR serialAVR(9600, 8, 0, 1, 1);		//Serial Port 1 is the hardware serial port

Display display;
Sound sound;
State state;

int main(){
	//Main program loop
	while (1){
		sound.poll();
		state.poll();
		display.update();

		delay_ms(50);	//This defines the blink speed
	}

	return 0;
}
