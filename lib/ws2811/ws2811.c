#include "ws2811.h"

#ifndef WS2811_PORT
#define WS2811_PORT PORTB
#endif

#ifndef WS2811_PIN
#define WS2811_PIN PINB1
#endif


// COMPA
#define T0H F_CPU / 1 / 4000000 // 0 code, high voltage time 250 ns
#define T1H F_CPU / 1 / 1000000 // 1 code, high voltage time 1000 ns
// COMPB
#define T F_CPU / 1 / 800000 // time 1250 ns
#define TRL F_CPU / 1 / 20000   // reset code, low voltage time 50 us

// @ 16 MHz, 4 clock cycles, 16 clock cycles, 800 clock cycles
// @ 20 MHz, 5 clock cycles, 20 clock cycles, 1000 clock cycles

#RES low voltage time Above 50µs 

ws2811_t* _b;	// the GRB bytes to send
uint8_t _s;		// the number of GRB bytes to send (must be a multiple of 3)
uint8_t _i;		// the byte position in the values array
uint8_t _j;		// the bit position in the current byte

void ws2811_init() {
	*(WS2811_PORT - 0x1) |= _BV(WS2811_PIN);	// DDR output
	
	TCCR0A = 0x0; 				// normal mode
	TCCR0B |= _BV(CS00);        // F_CPU / 1 prescaler
	OCR0A = F_CPU / 1 / baud;	// compare value
	TIMSK0 = 0x00;				// do not enable compare interrupt yet
	
	sei();
}
void ws2811_set(ws2811_t *values, uint8_t s) {
	_s = s * 3;
	_b = values;
	_i = 0;
	_j = 0;
	
	// reset
	PORTB &= ~(1 << _BV(WS2811_PIN));
	OCR0B = TRL;
	OCR0A = TRL+1;	// already low, no need for COMPA_vect to fire
	TIMSK0 = OCIE0A | OCIE0B;
	// the sending of bytes is handled in the ISR
}

uint8_t ws2811_ready() {
	return (_i >= _s) ? 1 : 0;
}

EMPTY_INTERRUPT(TIM0_OVF_vect)
ISR(TIM0_COMPA_vect) {
	PORTB &= ~(1 << _BV(WS2811_PIN));	// go low
}
ISR(TIM0_COMPB_vect) {
	TCNT0 = 0;
	
	uint8_t b = _b[_i];
	PORTB |= _BV(WS2811_PIN);			// go high
	OCR0A = (b & _BV(j++) > 0) ? T1H : T0H;
	if (_j >= 8) {
		_i++;
		_j= 0;
	}
	if (_i >= _s) {
		TIMSK0 = 0x00;
	}
}