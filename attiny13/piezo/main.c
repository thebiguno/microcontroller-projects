/*
 * Simple push/pull piezo driver.  Turned on by appliying logic high to 
 * pin 2 (PB3); piezo is driven from pins PB0 and PB1.
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

//Macro to convert VALUE (in µs) to a clock tick count with a hardcoded prescaler of 64.
#define PWM_US_TO_CLICKS(value) (F_CPU / 1000000) * (value / 64)

static void check_pin_state(){
	if (PINB & _BV(PINB3)){
		TCCR0B |= _BV(CS01) | _BV(CS00);
	}
	else {
		TCCR0B = 0;
	}
}

int main (void){
	//Set B0 and B1 to output
	DDRB |= _BV(PINB0) | _BV(PINB1);
	PORTB |= _BV(PINB0);

	//Set the PWM period (i.e. the buzzer frequency)
	OCR0A = PWM_US_TO_CLICKS(1500);
	
	//Select clock prescaler (to /64)
	TCCR0B |= _BV(CS01) | _BV(CS00);

	//Enable output compare 1 interrupt
	TIMSK0 = _BV(OCIE0A);
	
	//Set up pin B3 as a pin change interrupt
	PCMSK |= _BV(PCINT3);
	GIMSK |= _BV(PCIE);
	
	//Enable interrupts
	sei();

	check_pin_state();
	
	//Main program loop
	while (1){
		//Set sleep mode to 'power down' and enable sleep
		sleep_enable();
		sleep_cpu();
	
		_delay_ms(1000);
	}
}

ISR(TIM0_COMPA_vect){
	//XOR PWM pins
	PORTB ^= _BV(PINB0) | _BV(PINB1);
	
	//Reset counter
	TCNT0 = 0;
}

ISR(PCINT0_vect){
	check_pin_state();
}

