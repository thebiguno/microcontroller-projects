#include "alarmclock.h"

using namespace digitalcave;

//Current time.  Updated in state.cpp and used by display.cpp
time_t now;
tm_t now_tm;

I2CAVR* i2c;

void get_mcusr(void) __attribute__((naked))  __attribute__((section(".init3")));
void get_mcusr(void) {
	MCUSR = 0;
	wdt_disable();
}

int main() __attribute__((noreturn));

int main(){
	wdt_enable(WDTO_4S);

	state_init();
	display_init();

	DDRF |= _BV(PORTF4);	//Debugging to determine main loop frequency

	//Main program loop
	while (1){
		state_poll();

		display_update();

		wdt_reset();
	}
}
