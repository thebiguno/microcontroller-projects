#include <stdio.h>

#include "../status.h"

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
