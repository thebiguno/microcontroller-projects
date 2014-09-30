#include "util/delays.h"

//Defined in hardware/magnetometer.c
extern volatile uint8_t interval_do_magnetometer_reading;
void magnetometer_take_reading();

//Defined in hardware/battery.c
void battery_set_level();
extern volatile uint8_t battery_level;
extern uint8_t battery_status_enabled;

void delay_ms(uint16_t ms){
	double _ms = (double) ms;
	//Check various mailbox flags for stuff to do, and decrease ms accordingly
#if MAGNETOMETER == 1
	if (interval_do_magnetometer_reading){
		PORTD |= _BV(PORTD5);
		magnetometer_take_reading();
#if F_CPU == 12000000
		_ms -= 0.33;	//Determined from logic probes at 12MHz
#elif F_CPU == 20000000
		_ms -= 0.2;		//Approximation based on 12MHz one
#else
#error Unknown CPU speed; please update delay_ms
#endif
		interval_do_magnetometer_reading = 0x00;
		PORTD &= ~_BV(PORTD5);
	}
#endif
	
	if (battery_level){
		PORTD |= _BV(PORTD4);
		battery_set_level();
		//We only adjust ms if we are updating the status lights... otherwise, the extra delay is negligable
		if (battery_status_enabled){
#if F_CPU == 12000000
		_ms -= 0.22;		//Determined from logic probes at 12MHz
#elif F_CPU == 20000000
		_ms -= 0.13;		//Approximation based on 12MHz one
#else
#error Unknown CPU speed; please update delay_ms
#endif
		}
		PORTD &= ~_BV(PORTD4);
	}

	_delay_ms(_ms);
}