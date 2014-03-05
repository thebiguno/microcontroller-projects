/*
 * A manchester signal receiver.
 */

#include "manchester.h"
#include <avr/interrupt.h>

static uint8_t lower;	// 1/4
static uint8_t mid;   	// 3/4
static uint8_t upper;	// 5/4

static volatile uint8_t mbox;
static volatile uint8_t buf;
static volatile uint8_t data;

static volatile uint8_t bit_count;
static volatile uint8_t signal_state; // signal position in the bit (1 = T, 0 = 2T)
static volatile uint8_t packet_state; // byte position in the packet (1 = data, 0 = preamble)

void manchester_init_rx(uint16_t baud){
	TCCR0A = 0x0; 				// normal mode
	TCCR0B |= _BV(CS02);        // F_CPU / 256 prescaler
	
	uint8_t half = F_CPU / 255 / baud;
	lower = half / 2;
	mid = lower + half;
	upper = mid + half;

	EICRA |= _BV(ISC00);		// any logical change on int0
	EIMSK |= _BV(INT0);			// enable external interrupts on int0
	TIMSK0 = _BV(OCIE0A);		// enable timer
	
	sei();
}

void manchester_write_c(char data){
	//Nop loop to wait until last transmission has completed
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = data;
}


uint8_t manchester_available() {
	return mbox > 0;
}

uint8_t manchester_read(uint8_t *b) {
	while (!manchester_available());
	*b = data;
	mbox = 0;
	return 1;
}

static inline void readBit() {
	uint8_t input = PORTD & _BV(PIND6);
	
	if (packet_state == 0) {
		// preamble
		if (input) {
			bit_count++;
			if (bit_count > 7) bit_count = 7;
		} else {
			if (bit_count == 7) {
				// first zero after 7 or more ones; sync
				packet_state = 1;
			}
			bit_count = 0;
			buf = 0x00;
		}
	} else {
		// data
		if (input) {
			buf |= _BV(bit_count);
		}
		bit_count++;
		
		if (bit_count >= 8) {
			data = buf;
			mbox = 1;
			packet_state = 0;
			bit_count = 0;
			// TODO fire an interrupt
		}
	}
}

ISR(INT0_vect) {
	uint8_t ck = TCNT0;
	if (ck < lower) {
		// error
		signal_state = 0; // clear signal state
	} else if (ck < mid) {
		// T
		if (signal_state) {
			readBit();
		}
		signal_state ^= _BV(1); // toggle signal state
	} else if (ck < upper) {
		// 2T
		if (signal_state == 0) {
			readBit();
		} else {
			// error
			signal_state = 0; // clear signal state
		}
	} else {
		// error
		signal_state = 0;
	}
	
	TCNT0 = 0x00;
}



