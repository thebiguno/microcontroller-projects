#include "timer0.h"

using namespace digitalcave;

extern Buttons buttons;
extern volatile uint8_t buttons_held;
extern volatile uint8_t buttons_released;

void timer0_init(){
	//Enable timer 0 for button polling
	TCCR0A = 0x0;							//Normal mode
	TCCR0B |= _BV(CS02);					//256 prescaler
	TIMSK0 = _BV(TOIE0);					//Enable timer overflow interrupts
	
	sei();
}

ISR(TIMER0_OVF_vect){
	buttons.sample();
	buttons_held |= buttons.held();
	buttons_released |= buttons.released();
}