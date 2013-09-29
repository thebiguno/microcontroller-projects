#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint8_t ct; // a value from 0 to 143, odd numbers are rising edges
static volatile uint8_t tooth; // a value from 0 to 35

int main(void) {
	
	// set B0 and B1 as output
	DDRB = _BV(DDB1) | _BV(DDB0) | _BV(DDB2);
	
	// set up timers
	TCCR1A = 0x00;			// OC1A / OC1B disconnected
	TCCR1B = _BV(WGM12) | _BV(CS11);	// CLK / 8 (prescaler), clear timer on compare match
	TIMSK1 = _BV(OCIE1A);	// interrupt on compare A
	OCR1A = 255;//512;		// this is about 1800 rpm, but it will be changed on the first analog read
	
	// set up analog
	ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // enable, enable interrupt, prescale /128
	ADMUX = _BV(ADLAR);	// use AREF, left adjust result, read ADC0/PC0

	sei();
	
	ADCSRA |= _BV(ADSC);
	
	while(1) {
		; // do nothing, all code in ISRs
	}
}

// 16MHz = 0.0625 us per clock cycle, 0.5 us per timer1 interrupt

// analog ISR
ISR(ADC_vect){
	// 500 rpm / 60 = 8.3 Hz = 120 ms / 72 = 1666 us/event = timer1 value of 3332 (120 ms period)
	// 10000 rpm / 60 = 166.6 Hz = 6 ms / 72 = 83 us/event = timer1 value of 166 (6 ms period)
	
	// read ADC value
	uint8_t analog = ADCH;
 	OCR1A = ((uint32_t) ADCH * 3332 / 255) + 166;
}

ISR(TIMER1_COMPA_vect) {
	if (ct % 2 == 0) {
		tooth++;
		// rising edge
		if (tooth == 13 || tooth == 14 || tooth == 16 || tooth == 17 || tooth == 31 || tooth == 32) {
			// skip gap teeth
		} else {
			// crank event rising edge
			PORTB = _BV(PB0);
		}
		if ((ct > 71 && tooth == 15) || tooth == 33) {
			// cam event rising edge
			PORTB |= _BV(PB1);
		}
	} else {
		// falling edge
		PORTB = 0x00;
	}
	
	ct++;
	if (ct > 143) {
		ct = 0;
	}
	if (tooth > 35) {
		tooth = 0;
		PORTB |= _BV(PB2);
		
		// start ADC again -- read analong once per revolution
		ADCSRA |= _BV(ADSC);
	}
}