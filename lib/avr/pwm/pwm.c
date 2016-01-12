#include "pwm.h"

typedef struct pwm_pin_t {
	volatile uint8_t *port;
	uint8_t pin;
	uint16_t compare_value;
} pwm_pin_t;

typedef struct pwm_event_t {
	//Only used in OCR1B; ignored for high event in OCR1A.  This is the value which TCNT1 is firing 
	// on *now*; you need to reference the next element in the array to get the value which is 
	// to be reset for next time.
	uint16_t compare_value;

	//Port masks
#ifndef PWM_PORTA_UNUSED
	uint8_t porta_mask;
#endif
#ifndef PWM_PORTB_UNUSED
	uint8_t portb_mask;
#endif
#ifndef PWM_PORTC_UNUSED
	uint8_t portc_mask;
#endif
#ifndef PWM_PORTD_UNUSED
	uint8_t portd_mask;
#endif
} pwm_event_t;


volatile void* _pwm_events_low_ptr;								//Pointer to current value in _pwm_events_low.  Reset in OCR1A, incremented in OCR1B.


static uint8_t _set_phase_batch = 0;							//Set to 1 when set_phase_batch is called with a changed value.
static volatile uint8_t _set_phase = 0;							//Set to 1 when phase is re-calculated; signals that the ISR COMPA needs to re-load phase values.
static volatile uint8_t _set_phase_lock = 0; 					//Set to 1 when we are in set_phase function.  Prevents OCR1A from copying the double buffered pwm_events when this is 0.
static volatile uint8_t _set_stop = 0;							//Set to 1 when stop is requested.  ISR COMPA will turn off timer.

static uint16_t _set_period = 0; 								//New period defined; set in set_period, and updated to OCR1A in changed in COMPA interrupt

//Variables used to store config data
static pwm_pin_t _pwm_pins[PWM_MAX_PINS];						//Array of pins.  Index is important here, as that is how we refer to the pins from the outside world.
static uint8_t _count;											//How many pins should be used

static pwm_event_t _pwm_event_high;								//PWM event to set all pins high at start of period.  Calculated on a non-zero set_phase.
static pwm_event_t _pwm_event_high_new;							//Double buffer of pwm high event; copied to pwm_events in OCR1A when _set_phase is non-zero.

static pwm_event_t _pwm_events_low[PWM_MAX_PINS + 1];			//Array of pwm events.  Each event will set one or more pins low.
static pwm_event_t _pwm_events_low_new[PWM_MAX_PINS + 1];		//Double buffer of pwm events.  Calculated in each set_phase call; copied to pwm_events in OCR1A when _set_phase is non-zero.

static uint16_t _prescaler = 0x0;								//Numeric prescaler (1, 8, etc).  Required for _pwm_micros_to_clicks calls.
static uint8_t _prescaler_mask = 0x0;							//Prescaler mask corresponding to _prescaler


static uint16_t _pwm_micros_to_clicks(uint32_t micros){
	//There is a potential for an overflow here if micros * (F_CPU in MHz) does not fit into
	// a 32 bit variable.  If this happens, we can divide micros by prescaler before multiplying
	// with F_CPU, although this will give a loss of precision.  Leave as-is for now.
	return (((F_CPU / 1000000) * micros) / _prescaler) & 0xFFF0;
	
	//TODO Rounding, based on 10MHz clock and various prescalers:
	//Prescaler: 1, AND with 0xFF80
	//Prescaler: 8, AND with 0xFFF0
	//Prescaler: 64, AND with 0xFFFE
	//Other prescalers should be fine as-is...
	//More testing at different F_CPU values and prescalers are still required.
}

/*
 * Note: We extrapolate the DDR registers based off of the associated PORT 
 * register.  This assumes that the DDR registers come directly after the PORT
 * equivalents, with DDRX at the next address after PORTX.  This is valid for 
 * all the chips I have looked at; however, it is highly recommended that you 
 * check any new chips which you want to use this library with.
 */
void pwm_init(volatile uint8_t *ports[],
				uint8_t pins[],
				uint8_t count,
				uint32_t period) {

	_count = (count <= PWM_MAX_PINS ? count : PWM_MAX_PINS);
	
	//Store values
	for (uint8_t i = 0; i < _count; i++){
		_pwm_pins[i].port = ports[i];
		_pwm_pins[i].pin = pins[i];
		*(_pwm_pins[i].port - 0x1) |= _BV(_pwm_pins[i].pin);
	}
	
	//This is calculated by the focumula:
	// CUTOFF_VALUE = PRESCALER * MAX_VALUE / (F_CPU / 1000000)
	// where CUTOFF_VALUE is the period comparison for each if block,
	// PRESCALER is the prescaler in the datasheet for a given CSx selection,
	// and MAX_VALUE is the largest integer of the selected bit depth (256 / 65536).

#ifdef PWM_8_BIT
	uint32_t max_value = 255;
#else
	uint32_t max_value = 65535;
#endif
	
	if (period < (1 * max_value / (F_CPU / 1000000))){
		_prescaler = 1;
		_prescaler_mask = _BV(CS10);
	}
	else if (period < (8 * max_value / (F_CPU / 1000000))){
		_prescaler = 8;
		_prescaler_mask = _BV(CS11);	
	}
	else if (period < (64 * max_value / (F_CPU / 1000000))){
		_prescaler = 64;
		_prescaler_mask = _BV(CS11) | _BV(CS10);
	}
	else if (period < (256 * max_value / (F_CPU / 1000000))){
		_prescaler = 256;
		_prescaler_mask = _BV(CS12);
	}
	else if (period < (1024 * max_value / (F_CPU / 1000000))){
		_prescaler = 1024;
		_prescaler_mask = _BV(CS12) | _BV(CS10);
	}
	else {
		period = (1024 * max_value / (F_CPU / 1000000)); //The largest possible period
		_prescaler = 1024;
		_prescaler_mask = _BV(CS12) | _BV(CS10);
	}
				
	TCCR1A = 0x00;
	TCCR1B &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10));
	TCCR1B |= _prescaler_mask;
	
	//OCR1A controls the PWM period
	OCR1A = _pwm_micros_to_clicks(period);
	//OCR1B controls the PWM phase.  It is initialized later.
	
	//Enable compare interrupt on both channels
	TIMSK1 = _BV(OCIE1A) | _BV(OCIE1B);
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
	
	//Force interrupt on compare A initially
	TCCR1C |= _BV(FOC1A);
}

void pwm_start(){
	TCNT1 = 0x00;	//Restart timer counter
	TIMSK1 = _BV(OCIE1A) | _BV(OCIE1B);	//Enable output compare match interrupt enable
	TCCR1B &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10));
	TCCR1B |= _prescaler_mask;	//Enable
}

void pwm_stop(){
	_set_stop = 1;
}

//The comparison method used to sort pwm_pin variables
static int16_t _compare_values(const void *pin1, const void *pin2){
	 const pwm_pin_t *pwm1 = (const pwm_pin_t*) pin1;
	 const pwm_pin_t *pwm2 = (const pwm_pin_t*) pin2;
	 if (pwm1->compare_value < pwm2->compare_value) return -1;
	 else if (pwm1->compare_value > pwm2->compare_value) return 1;
	 else return 0;
}

void pwm_set_phase(uint8_t index, uint32_t phase){
	pwm_set_phase_batch(index, phase);
	pwm_apply_batch();
}

void pwm_set_phase_batch(uint8_t index, uint32_t phase){
	//Bounds checking
	if (index >= _count) {
		return;
	}
	
	//The new compare value, in clock ticks
	uint16_t new_clicks = _pwm_micros_to_clicks(phase);
	pwm_pin_t *p = &(_pwm_pins[index]);

	if (p->compare_value != new_clicks){
		_set_phase_batch = 1;
	}
	//Set the new compare value
	p->compare_value = new_clicks;
}

void pwm_apply_batch(){
	if (_set_phase_batch == 0){
		return;	//No need to re-calculate if there was no changed phases.
	}
	
	_set_phase_lock = 1;

	//Copy _pwm_pins to _pwm_pins_sorted, and then sort it by value
	pwm_pin_t _pwm_pins_sorted[PWM_MAX_PINS];
	for (uint8_t i = 0; i < _count; i++){
		_pwm_pins_sorted[i] = _pwm_pins[i];
	}
	qsort(_pwm_pins_sorted, _count, sizeof _pwm_pins_sorted[0], _compare_values);

	//Populate the _pwm_events_high variable and _pwm_events_low_new array, used in 
	// OCR1A and OCR1B respectively to turn pins on / off.
	//First we reset everything in this array...
#ifndef PWM_PORTA_UNUSED
	_pwm_event_high_new.porta_mask = 0x00;
#endif
#ifndef PWM_PORTB_UNUSED
	_pwm_event_high_new.portb_mask = 0x00;
#endif
#ifndef PWM_PORTC_UNUSED
	_pwm_event_high_new.portc_mask = 0x00;
#endif
#ifndef PWM_PORTD_UNUSED
	_pwm_event_high_new.portd_mask = 0x00;
#endif
	for (uint8_t i = 0; i < _count + 1; i++){
		_pwm_events_low_new[i].compare_value = 0xFFFF;
#ifndef PWM_PORTA_UNUSED
		_pwm_events_low_new[i].porta_mask = 0xFF;
#endif
#ifndef PWM_PORTB_UNUSED
		_pwm_events_low_new[i].portb_mask = 0xFF;
#endif
#ifndef PWM_PORTC_UNUSED
		_pwm_events_low_new[i].portc_mask = 0xFF;
#endif
#ifndef PWM_PORTD_UNUSED
		_pwm_events_low_new[i].portd_mask = 0xFF;
#endif
	}
	//... then we look through the sorted _pwm_pins list, and collect all of the ports / pins which are
	// set at the same compare values into a single event.
	uint16_t last_compare_value = _pwm_pins_sorted[0].compare_value;
	uint8_t last_index = 0;
	
	for (uint8_t i = 0; i < _count; i++){
		pwm_pin_t *p = &(_pwm_pins_sorted[i]);
		if (p->compare_value > last_compare_value){
			//We don't want an empty pwm_event at the beginning of the array, so verify whether last_compare_value was zero before incrementing
			if (last_compare_value > 0) {
				last_index++;
			}
			last_compare_value = p->compare_value;
		}

		if (p->compare_value > 0){
			//Set pins to high
#ifndef PWM_PORTA_UNUSED
			if (p->port == &PORTA) _pwm_event_high_new.porta_mask |= _BV(p->pin);
			else
#endif
#ifndef PWM_PORTB_UNUSED
			if (p->port == &PORTB) _pwm_event_high_new.portb_mask |= _BV(p->pin);
			else
#endif
#ifndef PWM_PORTC_UNUSED
			if (p->port == &PORTC) _pwm_event_high_new.portc_mask |= _BV(p->pin);
			else
#endif
#ifndef PWM_PORTD_UNUSED
			if (p->port == &PORTD) _pwm_event_high_new.portd_mask |= _BV(p->pin);
			else
#endif
			if (0);	//Nop so that ignored ports dont break things

			//Set pins to low
			pwm_event_t *e = &(_pwm_events_low_new[last_index]);
			e->compare_value = last_compare_value;
#ifndef PWM_PORTA_UNUSED
			if (p->port == &PORTA) e->porta_mask &= ~_BV(p->pin);
			else
#endif
#ifndef PWM_PORTB_UNUSED
			if (p->port == &PORTB) e->portb_mask &= ~_BV(p->pin);
			else
#endif
#ifndef PWM_PORTC_UNUSED
			if (p->port == &PORTC) e->portc_mask &= ~_BV(p->pin);
			else
#endif
#ifndef PWM_PORTD_UNUSED
			if (p->port == &PORTD) e->portd_mask &= ~_BV(p->pin);
			else
#endif
			if (0);	//Nop so that ignored ports dont break things
		}
	}

	_set_phase_batch = 0;

	//Signal OCR1A that we are ready to load new values
	_set_phase = 1;
	_set_phase_lock = 0;
}

void pwm_set_period(uint32_t period){
	_set_period = _pwm_micros_to_clicks(period);
}



/* 
 * The frequency comparison.  When it overflows, we reset the timer to 0.
 */
#ifdef PWM_8_BIT
EMPTY_INTERRUPT(TIM0_OVF_vect)
ISR(TIM0_COMPA_vect){
#else
EMPTY_INTERRUPT(TIMER1_OVF_vect)
ISR(TIMER1_COMPA_vect){
#endif
	//Update values if needed
	if (_set_phase && !_set_phase_lock){
		for (uint8_t i = 0; i < _count; i++){
			_pwm_events_low[i] = _pwm_events_low_new[i];
		}
		_pwm_event_high = _pwm_event_high_new;
		_set_phase = 0;
	}
	if (_set_period){
		OCR1A = _set_period;
		_set_period = 0;
	}
	if (_set_stop){
		TCCR1B = 0x00;
		
#ifndef PWM_PORTA_UNUSED
		PORTA &= ~_pwm_event_high.porta_mask;
#endif
#ifndef PWM_PORTB_UNUSED
		PORTB &= ~_pwm_event_high.portb_mask;
#endif
#ifndef PWM_PORTC_UNUSED
		PORTC &= ~_pwm_event_high.portc_mask;
#endif
#ifndef PWM_PORTD_UNUSED
		PORTD &= ~_pwm_event_high.portd_mask;
#endif
		
		_set_stop = 0;
		return;
	}
	//Reset counter after the new compare values are updated (otherwise it affects the phase 
	// of the next execution, causing jitter).
	TCNT1 = 0;	
	
	//Set to the first (sorted) compare value in the pwm_events_low array.  This is calculated 
	// in pwm_apply_batch(), and updated above in this ISR.
	_pwm_events_low_ptr = (void*) _pwm_events_low;
	OCR1B = _pwm_events_low[0].compare_value;
	
	//Set pins high.  We do this after re-enabling the clock so that we do not artificially increase 
	// the phase.  We turn off the ports (in COMPB) in the same order that we turn them on here,
	// so that the delta of any delay between PORTA and PORTD should be reduced or eliminated.
#ifndef PWM_PORTA_UNUSED
	PORTA |= _pwm_event_high.porta_mask;
#endif
#ifndef PWM_PORTB_UNUSED
	PORTB |= _pwm_event_high.portb_mask;
#endif
#ifndef PWM_PORTC_UNUSED
	PORTC |= _pwm_event_high.portc_mask;
#endif
#ifndef PWM_PORTD_UNUSED
	PORTD |= _pwm_event_high.portd_mask;
#endif
}

/* 
 * The phase comparison, implemented in C.  When it overflows, we find the next highest value.
 * For a faster alternative, comment this out and use the ASM version in pwm.S
 *
ISR(TIMER1_COMPB_vect){
	pwm_event_t* e = (pwm_event_t*) _pwm_events_low_ptr;
	_pwm_events_low_ptr = (uint8_t*) _pwm_events_low_ptr + sizeof(pwm_event_t);
	
#ifndef PWM_PORTA_UNUSED
	PORTA &= e->porta_mask;
#endif
#ifndef PWM_PORTB_UNUSED
	PORTB &= e->portb_mask;
#endif
#ifndef PWM_PORTC_UNUSED
	PORTC &= e->portc_mask;
#endif
#ifndef PWM_PORTD_UNUSED
	PORTD &= e->portd_mask;
#endif
	
	//Set the timer for the next lowest value.
	OCR1B = ((pwm_event_t*) _pwm_events_low_ptr)->compare_value;
}
*/
