#include "delays.h"

extern volatile uint8_t interval_do_magnetometer_reading;	//Defined in magnetometer_HMC5883L.c
extern volatile uint8_t interval_do_distance_reading;		//Defined in distance_HCSR04.c

void magnetometer_take_reading();
void distance_measure();

//Defined in hardware/battery.c
void battery_set_level();
extern volatile uint8_t battery_level;
extern uint8_t battery_status_enabled;

//Defined in Stubby.cpp
extern uint8_t pending_acknowledge;
extern uint8_t pending_complete;

void delay_ms(uint16_t ms){
	double _ms = (double) ms;
	//Check various mailbox flags for stuff to do, and decrease ms accordingly
#if MAGNETOMETER == 1
	if (interval_do_magnetometer_reading){
		magnetometer_take_reading();
#if F_CPU == 12000000
		_ms -= 0.33;	//Determined from logic probes at 12MHz
#elif F_CPU == 18432000
		_ms -= 0.22;	//Approximation based on 12MHz one
#elif F_CPU == 20000000
		_ms -= 0.2;		//Approximation based on 12MHz one
#else
#error Unknown CPU speed; please update delay_ms
#endif
		interval_do_magnetometer_reading = 0x00;
	}
#endif
	
	if (battery_level){
		battery_set_level();
		//We only adjust ms if we are updating the status lights... otherwise, the extra delay is negligable
		if (battery_status_enabled){
#if F_CPU == 12000000
		_ms -= 0.22;		//Determined from logic probes at 12MHz
#elif F_CPU == 18432000
		_ms -= 0.15;		//Approximation based on 12MHz one
#elif F_CPU == 20000000
		_ms -= 0.13;		//Approximation based on 12MHz one
#else
#error Unknown CPU speed; please update delay_ms
#endif
		}
	}
	
#if DISTANCE_SENSOR == 1
	if (interval_do_distance_reading){
		distance_measure();
		_ms -= 0.33;	//TODO Figure out average time to read distance sensor
		interval_do_distance_reading = 0x00;
	}
#endif

	
	if (pending_acknowledge){
		uint8_t data[1];
		data[0] = pending_acknowledge;
		protocol_send_message(MESSAGE_SEND_ACKNOWLEDGE, data, 1);
		pending_acknowledge = 0x00;
	}
	
	if (pending_complete){
		uint8_t data[1];
		data[0] = pending_complete;
		protocol_send_message(MESSAGE_SEND_COMPLETE, data, 1);
		pending_complete = 0x00;
	}

	_delay_ms(_ms);
}