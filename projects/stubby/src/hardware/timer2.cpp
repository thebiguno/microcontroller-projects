#include "timer2.h"

extern volatile uint8_t interval_do_magnetometer_reading;	//Defined in magnetometer_HMC5883L.c
extern volatile uint8_t interval_do_distance_reading;		//Defined in distance_HCSR04.c

void timer2_init(){
	//Set up the timer to run at F_CPU / 1024, in normal mode
	TCCR2A = 0x0;
	TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20);
	TIMSK2 = _BV(TOIE2);
}

EMPTY_INTERRUPT(TIMER2_COMPA_vect)
EMPTY_INTERRUPT(TIMER2_COMPB_vect)
ISR(TIMER2_OVF_vect){
	sei();
	
#if F_CPU == 12000000
	#define MAGNETOMETER_RESET			3
	#define BATTERY_RESET				31
	#define DISTANCE_RESET				63
#elif F_CPU == 18432000
	#define MAGNETOMETER_RESET			6
	#define BATTERY_RESET				63
	#define DISTANCE_RESET				127
#elif F_CPU == 20000000
	#define MAGNETOMETER_RESET			7
	#define BATTERY_RESET				63
	#define DISTANCE_RESET				127
#else
#warning Non-standard CPU speed.  Please specify a delay.
#endif	

	static uint8_t magnetometer_counter = 0;
	static uint8_t battery_counter = 0;
	static uint8_t distance_counter = 0;
	
	magnetometer_counter++;
	battery_counter++;
	distance_counter++;
	
	if (magnetometer_counter >= MAGNETOMETER_RESET){
		magnetometer_counter = 0;
		interval_do_magnetometer_reading = 0x01;
	}
	if (battery_counter >= BATTERY_RESET){
		battery_counter = 0;
		ADCSRA |= _BV(ADSC);		//Start ADC
	}
	if (distance_counter >= DISTANCE_RESET){
		distance_counter = 0;
		interval_do_distance_reading = 0x01;
	}
}
