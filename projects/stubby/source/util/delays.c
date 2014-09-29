#include "util/delays.h"

//Defined in hardware/magnetometer.c
extern volatile uint8_t interval_do_magnetometer_reading;
void magnetometer_take_reading();

//Defined in hardware/battery.c
void battery_set_level();
extern volatile uint8_t battery_level;						//Defined in battery.c



void delay_ms(uint16_t ms){
	//Check various mailbox flags for stuff to do, and decrease ms accordingly
#if MAGNETOMETER == 1
	if (ms > 1 && interval_do_magnetometer_reading){
		magnetometer_take_reading();
		ms--;	//TODO Figure out how long this actually takes
		interval_do_magnetometer_reading = 0x00;
	}
#endif
	
	if (ms > 1 && battery_level){
		battery_set_level();
		ms--;
	}

	_delay_ms(200);
}