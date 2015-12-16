#include "pwm.h"

typedef struct pwm_pin_t {
	volatile uint8_t *port;
	uint8_t pin;
	uint8_t compare_value;
} pwm_pin_t;

typedef struct pwm_event_t {
	//Only used in OCR1B; ignored for high event in OCR1A.  This is the value which TCNT1 is firing 
	// on *now*; you need to reference the next element in the array to get the value which is 
	// to be reset for next time.
	uint8_t compare_value;

	//Port masks
	uint8_t portb_mask;
	uint8_t portc_mask;
	uint8_t portd_mask;
	uint8_t porte_mask;
} pwm_event_t;


volatile void* _pwm_events_low_ptr;								//Pointer to current value in _pwm_events_low.  Reset in OCR1A, incremented in OCR1B.


static uint8_t _set_phase_batch = 0;							//Set to 1 when set_phase_batch is called with a changed value.
static volatile uint8_t _set_phase = 0;							//Set to 1 when phase is re-calculated; signals that the ISR COMPA needs to re-load phase values.
static volatile uint8_t _set_phase_lock = 0; 					//Set to 1 when we are in set_phase function.  Prevents OCR1A from copying the double buffered pwm_events when this is 0.
static volatile uint8_t _set_stop = 0;							//Set to 1 when stop is requested.  ISR COMPA will turn off timer.

static uint8_t _new_value = 0; 									//New period defined; set in set_period, and updated to OCR1A in changed in COMPA interrupt

//Variables used to store config data
static pwm_pin_t _pwm_pins[4];						//Array of pins.  Index is important here, as that is how we refer to the pins from the outside world.

static pwm_event_t _pwm_event_high;								//PWM event to set all pins high at start of period.  Calculated on a non-zero set_phase.
static pwm_event_t _pwm_event_high_new;							//Double buffer of pwm high event; copied to pwm_events in OCR1A when _set_phase is non-zero.

static pwm_event_t _pwm_events_low[4 + 1];			//Array of pwm events.  Each event will set one or more pins low.
static pwm_event_t _pwm_events_low_new[4 + 1];		//Double buffer of pwm events.  Calculated in each set_phase call; copied to pwm_events in OCR1A when _set_phase is non-zero.

/*
 * Note: We extrapolate the DDR registers based off of the associated PORT 
 * register.  This assumes that the DDR registers come directly after the PORT
 * equivalents, with DDRX at the next address after PORTX.  This is valid for 
 * all the chips I have looked at; however, it is highly recommended that you 
 * check any new chips which you want to use this library with.
 */
void pwm_init(volatile uint8_t* motorPort1, uint8_t motorPin1,
		volatile uint8_t* motorPort2, uint8_t motorPin2,
		volatile uint8_t* motorPort3, uint8_t motorPin3,
		volatile uint8_t* motorPort4, uint8_t motorPin4) {

	//Store values
	_pwm_pins[0].port = motorPort1;
	_pwm_pins[0].pin = motorPin1;
	*(_pwm_pins[0].port - 0x1) |= _BV(_pwm_pins[0].pin);

	_pwm_pins[1].port = motorPort2;
	_pwm_pins[1].pin = motorPin2;
	*(_pwm_pins[1].port - 0x1) |= _BV(_pwm_pins[1].pin);

	_pwm_pins[2].port = motorPort3;
	_pwm_pins[2].pin = motorPin3;
	*(_pwm_pins[2].port - 0x1) |= _BV(_pwm_pins[2].pin);

	_pwm_pins[3].port = motorPort4;
	_pwm_pins[3].pin = motorPin4;
	*(_pwm_pins[3].port - 0x1) |= _BV(_pwm_pins[3].pin);
	
	TCCR0A = 0x00;
	
	//Force interrupt on compare A initially, and set up div 8 prescaler
	TCCR0B |= _BV(FOC0A) | _BV(CS01);
	
	//The PWM period is controlled by overflow.
	//OCR1B controls the PWM phase.  It is initialized later.
	
	//Enable compare interrupt on overflow and A channel
	TIMSK0 = _BV(TOIE0) | _BV(OCIE0A);
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif
}

void pwm_start(){
	TCNT0 = 0x00;	//Restart timer counter
	TIMSK0 = _BV(OCIE0A) | _BV(OCIE0B);	//Enable output compare match interrupt enable
	TCCR0B |= _BV(CS01);	//Enable div 8 prescaler
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

void pwm_set_values(uint8_t motorValue1, uint8_t motorValue2, uint8_t motorValue3, uint8_t motorValue4){
	
	_pwm_pins[0].compare_value = motorValue1;
	_pwm_pins[1].compare_value = motorValue2;
	_pwm_pins[2].compare_value = motorValue3;
	_pwm_pins[3].compare_value = motorValue4;

	_set_phase_lock = 1;

	//Copy _pwm_pins to _pwm_pins_sorted, and then sort it by value
	pwm_pin_t _pwm_pins_sorted[4];
	for (uint8_t i = 0; i < 4; i++){
		_pwm_pins_sorted[i] = _pwm_pins[i];
	}
	qsort(_pwm_pins_sorted, 4, sizeof _pwm_pins_sorted[0], _compare_values);

	//Populate the _pwm_events_high variable and _pwm_events_low_new array, used in 
	// OCR0A and OCR0B respectively to turn pins on / off.
	//First we reset everything in this array...
	_pwm_event_high_new.portb_mask = 0x00;
	_pwm_event_high_new.portc_mask = 0x00;
	_pwm_event_high_new.portd_mask = 0x00;
	_pwm_event_high_new.porte_mask = 0x00;

	for (uint8_t i = 0; i < 4 + 1; i++){
		_pwm_events_low_new[i].compare_value = 0xFF;
		_pwm_events_low_new[i].portb_mask = 0xFF;
		_pwm_events_low_new[i].portc_mask = 0xFF;
		_pwm_events_low_new[i].portd_mask = 0xFF;
		_pwm_events_low_new[i].porte_mask = 0xFF;
	}
	//... then we look through the sorted _pwm_pins list, and collect all of the ports / pins which are
	// set at the same compare values into a single event.
	uint16_t last_compare_value = _pwm_pins_sorted[0].compare_value;
	uint8_t last_index = 0;
	
	for (uint8_t i = 0; i < 4; i++){
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
			if (p->port == &PORTB) _pwm_event_high_new.portb_mask |= _BV(p->pin);
			else if (p->port == &PORTC) _pwm_event_high_new.portc_mask |= _BV(p->pin);
			else if (p->port == &PORTD) _pwm_event_high_new.portd_mask |= _BV(p->pin);
			else if (p->port == &PORTE) _pwm_event_high_new.porte_mask |= _BV(p->pin);

			//Set pins to low
			pwm_event_t *e = &(_pwm_events_low_new[last_index]);
			e->compare_value = last_compare_value;

			if (p->port == &PORTB) e->portb_mask &= ~_BV(p->pin);
			else if (p->port == &PORTC) e->portc_mask &= ~_BV(p->pin);
			else if (p->port == &PORTD) e->portd_mask &= ~_BV(p->pin);
			else if (p->port == &PORTE) e->porte_mask &= ~_BV(p->pin);
		}
	}

	_set_phase_batch = 0;

	//Signal OCR0A that we are ready to load new values
	_set_phase = 1;
	_set_phase_lock = 0;
}


/* 
 * The frequency comparison.  When it overflows, we reset the timer to 0.
 */
EMPTY_INTERRUPT(TIMER0_COMPB_vect)
ISR(TIMER0_OVF_vect){
	//Update values if needed
	if (_set_phase && !_set_phase_lock){
		for (uint8_t i = 0; i < 4; i++){
			_pwm_events_low[i] = _pwm_events_low_new[i];
		}
		_pwm_event_high = _pwm_event_high_new;
		_set_phase = 0;
	}
	if (_new_value){
		OCR0A = _new_value;
		_new_value = 0;
	}
	if (_set_stop){
		TCCR0B = 0x00;
		
		PORTB &= ~_pwm_event_high.portb_mask;
		PORTC &= ~_pwm_event_high.portc_mask;
		PORTD &= ~_pwm_event_high.portd_mask;
		PORTE &= ~_pwm_event_high.porte_mask;
		
		_set_stop = 0;
		return;
	}
	//Reset counter after the new compare values are updated (otherwise it affects the phase 
	// of the next execution, causing jitter).
	TCNT0 = 0;	
	
	//Set to the first (sorted) compare value in the pwm_events_low array.  This is calculated 
	// in pwm_apply_batch(), and updated above in this ISR.
	_pwm_events_low_ptr = (void*) _pwm_events_low;
	OCR0B = _pwm_events_low[0].compare_value;
	
	//Set pins high.  We do this after re-enabling the clock so that we do not artificially increase 
	// the phase.  We turn off the ports (in COMPB) in the same order that we turn them on here,
	// so that the delta of any delay between PORTA and PORTD should be reduced or eliminated.
	PORTB |= _pwm_event_high.portb_mask;
	PORTC |= _pwm_event_high.portc_mask;
	PORTD |= _pwm_event_high.portd_mask;
	PORTE |= _pwm_event_high.porte_mask;
}

