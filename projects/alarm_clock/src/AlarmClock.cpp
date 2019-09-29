#include "AlarmClock.h"

using namespace digitalcave;

#ifdef DEBUG
SerialUSB serialUSB;
#endif

Display display;
Sound sound;
State state;

int main(){
	//Main program loop
	while (1){
		sound.poll();
		state.poll();
		display.update();

		delay_ms(50);	//No point in spinning the wheels too fast...
	}
}
