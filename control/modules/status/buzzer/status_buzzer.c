#include <stdio.h>

#include "../status.h"

//Macro to convert VALUE (in µs) to a clock tick count with a hardcoded prescaler of 1.
#define PWM_US_TO_CLICKS(value) (F_CPU / 1000000) * (value / 1)


void status_error_init(){
	*(&PORT_BUZZER - 0x1) |= _BV(PIN_BUZZER);	
	PORT_BUZZER &= ~_BV(PIN_BUZZER);
}

void status_error_battery(uint8_t error){
	if (error){
		PORT_BUZZER |= _BV(PIN_BUZZER);
	}
	else {
		PORT_BUZZER &= ~_BV(PIN_BUZZER);
	}
}
