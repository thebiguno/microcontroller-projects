#include "stubby.h"

leg_t legs[LEG_COUNT];

int main (void){
	double velocity = 0;
	double direction = 0;
	uint8_t special_allowed = 0x00;		//This sets to 0 every time a special is performed, and 1 every time a step is taken.  We can only do a special action when this is 1,

	comm_init();
	leg_init();
	
	status_set_color(255, 0, 0);

	while(1){
		comm_read(&velocity, &direction);
		
		if (special_allowed && comm_read_reset()){
			gait_init();
			special_allowed = 0x00;
		}
		else if (velocity < -0.5 || velocity > 0.5 || direction < -0.5 || direction > 0.5) {
			tripod_gait_step();
			special_allowed = 0x01;
			comm_reset_special();
		}
		else {
			leg_delay_ms(20);
		}
	}
}
