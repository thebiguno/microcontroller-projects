#include "alarmclock.h"

using namespace digitalcave;

int main(){
	state_init();
	display_init();

	//Main program loop
	while (1){
		state_poll();
		display_update();
	}

	return 0;
}
