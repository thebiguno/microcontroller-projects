#include <stdio.h>

#include "../status.h"

//Macro to convert VALUE (in µs) to a clock tick count with a hardcoded prescaler of 1.
#define PWM_US_TO_CLICKS(value) (F_CPU / 1000000) * (value / 1)


void status_error_init(){
	*(&PORT_BUZZER - 0x1) |= _BV(PIN_BUZZER_1) | _BV(PIN_BUZZER_2);
	
//	PORT_BUZZER &= ~(_BV(PIN_BUZZER_1) | _BV(PIN_BUZZER_2));
	PORT_BUZZER |= _BV(PIN_BUZZER_1);

	//_prescaler_mask = _BV(CS10);
	//_prescaler = 1;
//	TCCR1B |= _BV(CS10);
	
	//OCR1A controls the PWM period
//	OCR1A = PWM_US_TO_CLICKS(500);

	//Enable output compare 1
//	TIMSK1 = _BV(OCIE1A);
	
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
	
}

void status_error_battery(uint8_t error){
	if (error){
		//Set the PWM period (i.e. the buzzer frequency)
		OCR1A = PWM_US_TO_CLICKS(1000);
		
		//Select clock prescaler (to no prescaling)
		TCCR1B |= _BV(CS10);

		//Enable output compare 1 interrupt
		TIMSK1 = _BV(OCIE1A);
	
		//Force interrupt on compare A initially, and start.
//		TCCR1C |= _BV(FOC1A);
	}
	else {
		//Turn off clock
		TCCR1B = 0x00;
	}
	

}


ISR(TIMER1_COMPA_vect){
	//XOR PWM pins
	PORT_BUZZER ^= _BV(PIN_BUZZER_1) | _BV(PIN_BUZZER_2);
	
	//Reset counter
	TCNT1 = 0;
}