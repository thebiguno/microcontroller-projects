#include "alarmclock.h"

using namespace digitalcave;

//Current time.  Shared between state.cpp and display.cpp
time_t now;
tm_t now_tm;


int main() __attribute__((noreturn));

int main(){
	state_init();
	display_init();

	DDRF |= _BV(PORTF4);

	//Main program loop
	while (1){
		state_poll();

		display_update();
	}
}
