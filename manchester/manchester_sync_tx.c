#include "manchester.h"
#include <avr/interrupt.h>

static volatile uint8_t *port;
static volatile uint8_t *ddr;
static uint8_t pin;

static volatile uint8_t buf;
static volatile uint8_t bit_count;
static volatile uint8_t signal_state; // signal position in the bit (1 = T, 0 = 2T)

/*
 * Using timer 0 the baud rate can range from 300 to 62500 at 16 MHz
 */
void manchester_init_tx(volatile uint8_t *port, uint8_t pin, uint16_t baud){
	ddr = port - 0x1;
	*ddr |= _BV(pin);
	
	TCCR0A = 0x0; 				// normal mode
	TCCR0B |= _BV(CS02);        // F_CPU / 256 prescaler
	OCR0A = F_CPU / 256 / baud; // compare value
	TIMSK0 = _BV(OCIE0A);		// enable compare interrupt
	
	sei();
}

void write(uint8_t data) {
	while (TIMSK0 & _BV(OCIE0A));
	buf = data;
	bit_count = 0;
	signal_state = 0;
	TCNT0 = 0;					// reset timer
	TIMSK0 |= _BV(OCIE0A);		// enable timer
}

void manchester_write(uint8_t data) {
	write(0x7E);
	write(data);
}

ISR(TIMER0_COMPA_vect) {
	uint8_t bit = buf & _BV(bit_count);
	if (bit) {
		if (signal_state == 0) {
			*port &= ~_BV(pin); // low
		} else {
			*port |= _BV(pin);  // to high
		}
	} else {
		if (signal_state == 0) {
			*port |= _BV(pin);  // high
		} else {
			*port &= ~_BV(pin); // to low
		}
	}
	bit_count++;
	if (bit_count >= 8) {
		TIMSK0 &= ~_BV(OCIE0A);		// disable timer
	}
}

EMPTY_INTERRUPT(TIMER0_COMPB_vect)
EMPTY_INTERRUPT(TIMER0_OVF_vect)

