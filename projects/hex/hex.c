#include "hex.h"

int main (void){
	servo_init();
	leg_init();

	//tripod_gait_init();
	//wave_gait_init();
	ripple_gait_init();
	
	//int offset = 0;
	while(1){
		ripple_step();
	}
}
