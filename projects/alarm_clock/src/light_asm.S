#include <avr/io.h>

.global TIMER1_COMPA_vect
TIMER1_COMPA_vect:
	CBI			_SFR_IO_ADDR(PORTD),				5;
	RETI

.global TIMER1_COMPB_vect
TIMER1_COMPB_vect:
	CBI			_SFR_IO_ADDR(PORTD),				6;
	RETI

.global TIMER1_COMPC_vect
TIMER1_COMPC_vect:
	CBI			_SFR_IO_ADDR(PORTD),				7;
	RETI
