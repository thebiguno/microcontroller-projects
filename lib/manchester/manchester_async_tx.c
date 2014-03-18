#include "manchester.h"
#include <avr/interrupt.h>

static volatile uint8_t *_port;
static volatile uint8_t *_ddr;
static uint8_t _pin;

static volatile uint8_t buf;
static volatile uint8_t bit_count;
static volatile uint8_t signal_state; // Signal position in the bit (0 = 0T / 2T, 1 = T).  Always send 

/*
 * Using timer 0 the baud rate can range from 300 to 62500 at 16 MHz
 */
void manchester_init_tx(volatile uint8_t *port, uint8_t pin, uint16_t baud){
	_port = port;
	_pin = pin;
	_ddr = port - 0x1;
	*_ddr |= _BV(pin);	//Enable output on selected pin
	
	TCCR0A = 0x0; 				// normal mode
	TCCR0B |= _BV(CS02);        // F_CPU / 256 prescaler
	OCR0A = F_CPU / 256 / baud; // compare value
	TIMSK0 = 0x00;		// do not enable compare interrupt yet; this is done in write()
	
	sei();
}

void write(uint8_t data) {
	buf = data;
	TCNT0 = 0x00;				// reset timer counter
	bit_count = 0;
	signal_state = 0;
	TIMSK0 |= _BV(OCIE0A);			// enable compare a interrupt
	
	while (TIMSK0 & _BV(OCIE0A));	//Wait until the byte has been sent
}

void manchester_write(uint8_t data) {
	*_port |= _BV(_pin);			// set output high
	write(0x7F);
	write(data);
}

ISR(TIMER0_COMPA_vect) {
	TCNT0 = 0x00;		//reset timer counter

	if (bit_count <= 7){
		uint8_t bit = (buf & _BV(bit_count));
		
		if (bit) {
			//We want to send a 1 bit; first step is to move the signal low, so that
			// we can transition from low to high on the next clock cycle.
			if (signal_state == 0) {
				*_port &= ~_BV(_pin); // set signal low at 0T
			} else {
				*_port |= _BV(_pin);  // move to high at 1T
			}
		} else {
			//We want to send a 0 bit; first step is to move the signal high, so that
			// we can transition from high to low on the next clock cycle.
			if (signal_state == 0) {
				*_port |= _BV(_pin);  // set signal high at 0T
			} else {
				*_port &= ~_BV(_pin); // move to low at 1T
			}
		}
	}
	else if (bit_count == 8){
		//Send a 0 stop bit.
		if (signal_state == 0) {
			*_port |= _BV(_pin);  // set signal high at 0T
		} else {
			*_port &= ~_BV(_pin); // move to low at 1T
		}
	}
	
	signal_state ^= 0x01;
	
	if (signal_state == 0) bit_count++;
	
	if (bit_count > 8) {
		TIMSK0 &= ~_BV(OCIE0A);		// disable timer
		*_port &= ~_BV(_pin);	// set output high
		//PORTD = 0x00;	//TODO
	}
}

EMPTY_INTERRUPT(TIMER0_COMPB_vect)
EMPTY_INTERRUPT(TIMER0_OVF_vect)
