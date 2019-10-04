/*
 * Low resolution implementation of timer using a 8 bit clock with prescaler,
 * etc.  Clicks approximately every millisecond.  Sub-millisecond resolution is available
 * on demand, and has approximately 12µs resolution at 20MHz.
 * You can use this file when you don't need high resolution timing, and don't care
 * about slight clock drift, inaccuracy, etc.  For timing most human-related activities,
 * though, it should be more than sufficient.
 */
#include "timer.h"

#if TIMER_BITS == 64
static volatile uint64_t _timer_millis;
#elif TIMER_BITS == 32
static volatile uint32_t _timer_millis;
#endif
static uint32_t _timer_micro_divisor;

/*
 * Initializes the timer, and resets the timer count to 0.  Sets up the ISRs
 * linked with timer0.
 */
void timer_init(){
	//Set up the timer to run at F_CPU / 256, in normal mode (we reset TCNT0 in the ISR)
	TCCR0A = 0x0;
	TCCR0B |= _BV(CS02);

	//Every _timer_micro_divisor clock ticks is one microsecond.
	_timer_micro_divisor = F_CPU / 1000000;

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

#elif defined(__AVR_ATmega48__)   || \
    defined(__AVR_ATmega48P__)    || \
	defined(__AVR_ATmega168__)   || \
	defined(__AVR_ATmega328__)     || \
	defined(__AVR_ATmega328P__)    || \
	defined(__AVR_ATmega324P__)    || \
	defined(__AVR_ATmega644__)     || \
	defined(__AVR_ATmega644P__)    || \
	defined(__AVR_ATmega644PA__)   || \
	defined(__AVR_ATmega1284P__)   || \
	defined(__AVR_ATmega32U2__)    || \
	defined(__AVR_ATmega16U4__)    || \
	defined(__AVR_ATmega32U4__)

	TIMSK0 = _BV(OCIE0A);

#endif


	//Reset count variables
	_timer_millis = 0;

	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
}

/*
 * Returns the number of milliseconds which have elapsed since the
 * last time timer_init() was called.  Overflows after about 49 days.
 */
#if TIMER_BITS == 64
uint64_t timer_millis(){
#elif TIMER_BITS == 32
uint32_t timer_millis(){
#endif
	return _timer_millis;
}

/*
 * Returns the number of microseconds which have elapsed since the
 * last time timer_init() was called.  Overflows after about 71 minutes.
 */
#if TIMER_BITS == 64
uint64_t timer_micros(){
#elif TIMER_BITS == 32
uint32_t timer_micros(){
#endif
	//TCNT0 is a value between 0 and 78 (assuming a 20MHz clock; see OCR0A for where this is calculated), and is
	// the total number of clock ticks in one millisecond.
	//We have a theoretical resolution of about 12 microseconds (at 20MHz) here; in practice it will be less
	// since the division itself will take a fair bit of time.
	return (_timer_millis * 1000) + (((uint16_t) TCNT0 * 256) / _timer_micro_divisor);
}


/*
 * The ISR for timer0 overflow.  Increment the _timer_count here, and do the calculcations
 * to increment _timer_millis as needed.
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
#elif defined(__AVR_ATmega168__)   || \
	defined(__AVR_ATmega328__)     || \
	defined(__AVR_ATmega328P__)    || \
	defined(__AVR_ATmega324P__)    || \
	defined(__AVR_ATmega644__)     || \
	defined(__AVR_ATmega644P__)    || \
	defined(__AVR_ATmega644PA__)   || \
	defined(__AVR_ATmega1284P__)   || \
	defined(__AVR_ATmega32U2__)    || \
	defined(__AVR_ATmega16U4__)    || \
	defined(__AVR_ATmega32U4__)
EMPTY_INTERRUPT(TIMER0_COMPB_vect)
EMPTY_INTERRUPT(TIMER0_OVF_vect)
ISR(TIMER0_COMPA_vect){
#else //TODO Verify which AVRs use this ISR, and print error accordingly
	#error You must define TIMER0 vectors for your chip!  Please verify that MMCU is set correctly, and that there is a matching vector definition in timer_0.c
	timer_0_error(){
#endif
	TCNT0 = 0;
	_timer_millis++;
}
