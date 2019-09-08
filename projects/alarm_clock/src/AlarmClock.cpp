#include "AlarmClock.h"

using namespace digitalcave;

#ifdef DEBUG
SerialUSB serial;
#endif

int main(){
	State state;
	Display display;

	//Main program loop
	while (1){
		state.poll();

		display.update(state);

		delay_ms(50);	//No point in spinning the wheels too fast...
	}
}
