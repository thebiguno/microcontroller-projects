#include "timer.h"
#include "../button/button.h"

static volatile uint32_t _timer_millis;

/*
 * Initializes the timer, and resets the timer count to 0.  Sets up the ISRs 
 * linked with timer0.
 */
void timer_init(){
	//Set up the timer to run at F_CPU / 256, in normal mode (we reset TCNT0 in the ISR)
	TCCR0A = 0x0;
	TCCR0B |= _BV(CS02);
	
	//Set compare value to be F_CPU / 1000 -- fire interrupt every millisecond
	OCR0A = F_CPU / 256 / 1000;
	
	//Enable compare interrupt
#if defined(__AVR_ATtiny13__)   || \
	defined(__AVR_ATtiny24__)   || \
	defined(__AVR_ATtiny44__)   || \
	defined(__AVR_ATtiny84__)   || \
	defined(__AVR_ATtiny25__)   || \
	defined(__AVR_ATtiny45__)   || \
	defined(__AVR_ATtiny85__)

	TIMSK = _BV(OCIE0A);
	
#elif defined(__AVR_ATtiny2313__)  || \
	defined(__AVR_ATmega168__)     || \
	defined(__AVR_ATmega328__)     || \
	defined(__AVR_ATmega328P__)    || \
	defined(__AVR_ATmega324P__)    || \
	defined(__AVR_ATmega644__)     || \
	defined(__AVR_ATmega644P__)    || \
	defined(__AVR_ATmega644PA__)   || \
	defined(__AVR_ATmega1284P__)
	
	TIMSK0 = _BV(OCIE0A);
	
#endif
	

	//Reset count variables
	_timer_millis = 0;
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
	
	DDRB = 0xFF;
}

/*
 * Returns the number of milliseconds which have elapsed since the 
 * last time timer_init() was called.
 */
uint64_t timer_millis() {
	return _timer_millis;
}

/* 
 * The ISR for timer0 overflow.  Increment the _timer_count here, and do the calculcations
 * to increment _timer_millis as needed.
 * 
 */
#if defined(__AVR_ATtiny13__)   || \
	defined(__AVR_ATtiny24__)   || \
	defined(__AVR_ATtiny44__)   || \
	defined(__AVR_ATtiny84__)   || \
	defined(__AVR_ATtiny25__)   || \
	defined(__AVR_ATtiny45__)   || \
	defined(__AVR_ATtiny85__)
EMPTY_INTERRUPT(TIM0_COMPB_vect)
EMPTY_INTERRUPT(TIM0_OVF_vect)
ISR(TIM0_COMPA_vect){
#elif defined(__AVR_ATtiny2313__)  || \
	defined(__AVR_ATmega168__)     || \
	defined(__AVR_ATmega328__)     || \
	defined(__AVR_ATmega328P__)    || \
	defined(__AVR_ATmega324P__)    || \
	defined(__AVR_ATmega644__)     || \
	defined(__AVR_ATmega644P__)    || \
	defined(__AVR_ATmega644PA__)   || \
	defined(__AVR_ATmega1284P__)
EMPTY_INTERRUPT(TIMER0_COMPB_vect)
EMPTY_INTERRUPT(TIMER0_OVF_vect)
ISR(TIMER0_COMPA_vect){
#else //TODO Verify which AVRs use this ISR, and print error accordingly
	#error You must define TIMER0 vectors for your chip!  Please verify that MMCU is set correctly, and that there is a matching vector definition in timer_0.c
	timer_0_error(){
#endif
	TCNT0 = 0;
	_timer_millis++;
	if (_timer_millis > 86400000) _timer_millis = 0;
	
	button_sample();
}
