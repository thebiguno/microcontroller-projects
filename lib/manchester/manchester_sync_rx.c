/*
 * A manchester signal receiver.
 */

#include "manchester.h"
#include <avr/interrupt.h>

#define MANCHESTER_BUFFER_SIZE 40
// messages are escaped so that they never contain ff, fe, or 7d
// ff and fe are escaped so that we don't mistake real data for a preamble if the current state is to listen for preamble

#define ESCAPE 0x7d

static uint16_t _lower;	// 1/4
static uint16_t _mid;   	// 3/4
static uint16_t _upper;	// 5/4

static volatile uint8_t _buf[MANCHESTER_BUFFER_SIZE];
static volatile uint8_t _sig;		// current signal position in the bit (1 = T, 0 = 2T)
static volatile uint8_t _bit;		// current bit position in the byte
static volatile uint8_t _pos;		// current byte position in the frame
static volatile uint8_t _len;		// current frame length
static volatile uint8_t _b;			// current byte being read
static volatile uint8_t _chk;		// running checksum
static volatile uint8_t _rdy;		// 0 = not at the end of a frame, > 0 = number of bytes available to read from the frame
static volatile uint8_t _st; 		// current frame state: 0 = listening for preamble, 1 = reading byte, 2 = reading escaped byte

void manchester_init_rx(uint16_t baud) {
	TCCR0A = 0x0; 				// normal mode
	TCCR0B |= _BV(CS02) | _BV(CS00);        // F_CPU / 256 prescaler
	
	uint8_t half = F_CPU / 1024 / baud;	//52 at 16MHz and 300 baud
	_lower = half / 2;			//26 at "
	_mid = lower + half;			//78 at "
	_upper = mid + half;			//130 at "

	EICRA |= _BV(ISC00);		// any logical change on int0
	EIMSK |= _BV(INT0);			// enable external interrupts on int0
	
	sei();
}

uint8_t manchester_available() {
	return _rdy;
}

/*
 * Copies data from the buffer to the destination, dst should be at least MANCHESTER_BUFFER_SIZE long.
 * Returns the number of bytes copied
 */ 
uint8_t manchester_read(uint8_t dst[]) {
	while (!manchester_available());  // block until a frame has been read
	uint8_t result = _rdy;
	_rdy = 0;
	for (uint8_t i = 0; i < result; i++) {
		dst[i] = _buf[i];
	}
	return result;
}

static inline void readBit() {
	uint8_t input = PIND & _BV(PIND2);
	
	if (input) PORTC |= _BV(PORTC2);	//TODO remove
	else PORTC &= ~_BV(PORTC2);			//TODO remove
	
	if (_st == 0) {
		// preamble
		if (input) {
			_bit++;
			if (_bit > 7) _bit = 7;
		} else {
			if (_bit >= 7) {
				// first zero after 7 or more ones; sync
				_st = 1;
				PORTC |= _BV(PORTC5);	//TODO remove
			}
			_bit = 0;
			_b = 0x00;
		}
	} else {
		// data
		if (input) {
			_b |= _BV(_bit);
		}
		_bit++;
		
		if (_bit >= 8) {
			b = _b;
			_bit = 0;
			//TIMSK0 &= ~_BV(OCIE0A);			// disable timer
			//TCNT0 = 0x00;					// reset timer counter
			PORTC &= ~_BV(PORTC5);			// TODO remove
			
			if (b == ESCAPE) {
				_st = 2;
				return;
			}
			if (_st == 2) {
				b = 0x20 ^ b;
				_st = 1;
			}
			
			if (_pos == 0) {
				// length byte
				_len = b;
				_pos++;
				return;
			} else {
				// data byte
				_chk += b;
				if (_pos == (_len + 1)) {
					if (_chk == 0xff) {
						_rdy = _len;
					} else {
						_st = 0;
					}
					_pos = 0;
					_chk = 0;
				} else {
					_buf[_pos - 1] = b;
					_pos++;
				}
			}
		}
	}
}

ISR(INT0_vect) {
	PORTC ^= _BV(PORTC0);		//TODO remove
	TIMSK0 |= _BV(OCIE0A);		// enable timer
	
	uint8_t ck = TCNT0;
	if (ck < _lower) {
		// error
		_sig = 0; // clear signal state
	} else if (ck < _mid) {
		// T
		if (_sig) {
			PORTC ^= _BV(PORTC1);	//TODO
			readBit();
		}
		_sig ^= _BV(1); // toggle signal state
	} else if (ck < _upper) {
		// 2T
		if (_sig == 0) {
			readBit();
			PORTC ^= _BV(PORTC1);	//TODO
		}
		else {
			_sig ^= _BV(1); // toggle signal state
		}
	} else {
		// error / start of message; reset state
		_sig = 0;
	}
	
	TCNT0 = 0x00;
}

EMPTY_INTERRUPT(TIMER0_COMPA_vect)
EMPTY_INTERRUPT(TIMER0_COMPB_vect)
EMPTY_INTERRUPT(TIMER0_OVF_vect)


