#include "timer1.h"

static volatile uint8_t tick = 0;

void timer1_init(){
	//Set up the timer to run at F_CPU/1024 in CTC mode
	TCCR1A = 0x0;
	TCCR1B |= _BV(CS12) | _BV(CS10) | _BV(WGM12);
	
	//Set compare value to be F_CPU (with a 1024 prescaler) divided by 4 -- fire interrupt every 1/4s (250ms)
	OCR1A = F_CPU / 1024 / 4;
	
	//Enable compare interrupt
	TIMSK1 = _BV(OCIE1A);

	sei();
}

uint8_t timer1_next_tick(){
	uint8_t result = tick;
	if (result) tick = 0;
	return result;
}

EMPTY_INTERRUPT(TIMER1_COMPB_vect)
EMPTY_INTERRUPT(TIMER1_OVF_vect)
ISR(TIMER1_COMPA_vect){
	tick++;
}