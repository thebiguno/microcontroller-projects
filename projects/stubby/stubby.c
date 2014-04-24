#include "stubby.h"

leg_t legs[LEG_COUNT];

int main (void){
	double velocity = 0;
	double direction = 0;
	uint8_t special = 0x00;
	uint8_t special_allowed = 0x00;		//This sets to 0 every time a special is performed, and 1 every time a step is taken.  We can only do a special action when this is 1,
	uint8_t controller_init = 0x00;		//Whenever this resets to 0, we re-send controler init messages
	uint8_t repeat_counter = 0x00;		//Whenever this gets to 255, we invalidate all current messages.  On receipt of new message, we reset to 0.

	comm_init();
	leg_init();

	while(1){
		//Every few seconds we will send these controller init messages.
		if (controller_init == 0x00){
			serial_write_b(0x41);	//Enable analog sticks
			serial_write_b(0xFF);	//Set an analog repeat time of about 32ms.
		}
		controller_init++;

		if (comm_available()){
			comm_read(&velocity, &direction, &special);
		}
		else {
			repeat_counter++;
			if (repeat_counter >= 0x0F){
				velocity = 0;
				direction = 0;
				special = 0x00;
			}
		}
		
		if (special_allowed && (special & SPECIAL_RESET)){
			gait_init();
			special_allowed = 0x00;
		}
		else if (velocity < -0.1 || velocity > 0.1 || direction < -0.1 || direction > 0.1) {
			gait_step(velocity, direction);
			special_allowed = 0x01;
		}
		else {
			leg_delay_ms(20);
		}
	}
}
