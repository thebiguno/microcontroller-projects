#include <avr/io.h>
#include <avr/interrupt.h>
#include "lib/usb/rawhid.h"

static volatile uint8_t ct; // a value from 0 to 143, odd numbers are rising edges
static volatile uint8_t tooth; // a value from 0 to 35

uint8_t rx_buffer[3];

int main(void) {
	usb_init();
	
	// set B0 as output for crank simulation
	// set B1 as output for cam samulation
	// set B2 as output for start of revolution
	// set B3 as output for change trigger
	// set B7 as output for throttle position simulation fast pwm (OCR0A)
	DDRB = _BV(DDB0) | _BV(DDB1) | _BV(DDB2) | _BV(DDB3) | _BV(DDB7);
	
	// set up timer0 for throttle position fast pwm, clk/8
	TCCR0A = _BV(COM0A1) | _BV(WGM01) | _BV(WGM00);
	TCCR0B = _BV(CS01);
	
	// set up timer1 for crank and cam square output
	TCCR1A = 0x00;			// OC1A / OC1B disconnected
	TCCR1B = _BV(WGM12) | _BV(CS11);	// CLK / 8 (prescaler), clear timer on compare match
	TIMSK1 = _BV(OCIE1A);	// interrupt on compare A
	OCR1A = 255;//512;		// this is about 1800 rpm, but it will be changed on the first analog read
	
	sei();
	
	while(1) {
		if (usb_configured()) {
			if (usb_rawhid_recv(rx_buffer, 0) > 0) {
				OCR0A = rx_buffer[0];

				// 500 rpm / 60 = 8.3 Hz = 120 ms / 72 = 1666 us/event = timer1 value of 3332 (120 ms period)
				// 10000 rpm / 60 = 166.6 Hz = 6 ms / 72 = 83 us/event = timer1 value of 166 (6 ms period)
				OCR1AH = rx_buffer[1];
				OCR1AL = rx_buffer[2];
			}
		}
	}
}

// 16MHz = 0.0625 us per clock cycle, 0.5 us per timer1 interrupt
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
	}
}
