#include "pwm.h"

uint8_t pwm_set(volatile uint8_t *port, volatile uint8_t *ddr, uint8_t pin, uint8_t duty_cycle){

//This is ugly -- since each AVR chip will have different features, this is 
// enabled on a per-chip basis :-(  We will start with common chips defined,
// if the one you are interested in is not defined, you need to add a 
// definition here.
#if defined(__AVR_ATmega168__)
	if (*port == PORTD && pin == 6){ //OC0A
		//Clear on compare match when up-counting; set when down-counting.
		TCCR0A |= _BV(COM0A0);
		TCCR0A &= ~(_BV(COM0A1));

		//Set Fast PWM mode
		TCCR0A |= _BV(WGM00) | _BV(WGM01);
		TCCR0B |= _BV(WGM02);
		
		//No prescaler
		TCCR0B |= _BV(CS00);
		
		//Enable output mode
		*ddr |= _BV(pin);
		
		OCR0A = duty_cycle;
	}
	else if (*port == PORTD && pin == 5){ //OC0B
		TCCR0A |= _BV(COM0B1);
		OCR0B = duty_cycle;
	}
	else if (*port == PORTB && pin == 1){ //OC1A
		TCCR1A |= _BV(COM1A1);
		OCR1A = duty_cycle;
	}
	else if (*port == PORTB && pin == 2){ //OC1B
		TCCR1A |= _BV(COM1B1);
		OCR1B = duty_cycle;
	}
	else if (*port == PORTB && pin == 3){ //OC2A
		TCCR2A |= _BV(COM2A1);
		OCR2A = duty_cycle;
	}
	else if (*port == PORTD && pin == 3){ //OC2B
		TCCR2A |= _BV(COM2B1);
		OCR2B = duty_cycle;
	}
	else{
		return 0xFF;
	}
	
	return 0x00;
//#elif defined(__AVR_ATmega644__)
#else
	#error "Unknown AVR chip; PWM hardware cannot be initialized.  Make sure MCPU is set, or add apropriate hardware setup to PWM library."
#endif
	
}
