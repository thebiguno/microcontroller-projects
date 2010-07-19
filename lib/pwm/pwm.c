//TODO We should probably time how long it takes to run the timer interrupts, and compensate
// for the lag.  This will mostly affect high frequency PWM, and will manifest itself 
// in the form of slightly lower frequencies than you would expect.  (From experiments,
// a 2ms period (which should be 500Hz) registers as 485Hz).

#include "pwm.h"

//Macro to convert VALUE (in µs) to a clock tick count with a specified prescaler.
#define PWM_US_TO_CLICKS(value,prescaler) (F_CPU / 1000000) * (value / prescaler)

//Current values are here
static uint16_t _values[PWM_MAX_PINS];

//New values are here, they are copied over at the start of a new waveform
static uint16_t _new_values[PWM_MAX_PINS];
static uint8_t _new_value_set = 0; //Set to true when updated values
static uint32_t _new_period = 0; //New period defined, changed in COMPA interrupt

//Variables used to store config data
static volatile uint8_t *_ports[PWM_MAX_PINS];		//Array of ports used
static volatile uint8_t *_ddrs[PWM_MAX_PINS];		//Array of DDRs used
static uint8_t _pins[PWM_MAX_PINS];				//Array of pins used
static uint8_t _count;								//How many pins should be used

static uint16_t _prescaler = 0x0;
static uint8_t _prescaler_mask = 0x0;

void pwm_init(volatile uint8_t *ports[],
				volatile uint8_t *ddrs[],
				uint8_t pins[],
				uint8_t count,
				uint32_t period) {

	_count = (count <= PWM_MAX_PINS ? count : PWM_MAX_PINS);
	
	//Store values
	for (uint8_t i = 0; i < _count; i++){
		_ports[i] = ports[i];
		_ddrs[i] = ddrs[i];
		_pins[i] = pins[i];
	}
	
	//Rule of thumb, using 20MHz as the largest F_CPU value.  If running on
	// a slower clock, speeds can probably be tweaked, but this is probably
	// good enough.
	if (period < 3200){
		_prescaler = 1;
		_prescaler_mask = _BV(CS10);
	}
	else if (period < 25600){
		_prescaler = 8;
		_prescaler_mask = _BV(CS11);	
	}
	else if (period < 204800){
		_prescaler = 64;
		_prescaler_mask = _BV(CS11) | _BV(CS10);
	}
	else if (period < 838656){
		_prescaler = 256;
		_prescaler_mask = _BV(CS12);
	}
	else if (period < 3354624){
		_prescaler = 1024;
		_prescaler_mask = _BV(CS12) | _BV(CS10);
	}
	else {
		period = 3354624; //The largest possible period (given 20MHz clock)
		_prescaler = 1024;
		_prescaler_mask = _BV(CS12) | _BV(CS10);
	}
	
	//Initialize _values and _new_values to 0, and enable all pins for output
	for (uint8_t i = 0; i < _count; i++){
		*_ddrs[i] |= _BV(_pins[i]);
		_values[i] = 0;
		_new_values[i] = 0;
	}
				
	//Set up the timer to run at F_CPU / 256 (prescaler), in normal mode.  You can 
	// change the prescaler to get a different frequency range, but make sure you 
	// adjust the PWM_US_TO_CLICKS macro as well.
	TCCR1A = 0x0;
	TCCR1B |= _prescaler_mask;
	
	//OCR1A controls the PWM period
	OCR1A = PWM_US_TO_CLICKS(period, _prescaler);
	//OCR1B controls the PWM phase.  It is initialized later.
	
	//Enable compare interrupt on both channels
	TIMSK1 = _BV(OCIE1A) | _BV(OCIE1B);
	
	//Enable interrupts
	sei();
	
	//Force interrupt on compare A initially
	TCCR1C |= _BV(FOC1A);	
}

void pwm_start(){
	TCNT1 = 0;
	TIMSK1 = _BV(OCIE1A) | _BV(OCIE1B);
	TCCR1B |= _prescaler_mask;
}

void pwm_stop(){
	TCCR1B = 0x00;
	TIMSK1 = _BV(OCIE1A) | _BV(OCIE1B);
}

void pwm_set_phase(uint8_t index, uint16_t phase){
	if (index < _count){
		_new_values[index] = PWM_US_TO_CLICKS(phase, _prescaler);
		_new_value_set = 1;
	}
}

void pwm_set_period(uint32_t period){
	_new_period = period;
}

/* 
 * The frequency comparison.  When it overflows, we reset the timer to 0.
 */
ISR(TIMER1_COMPA_vect){
	//Turn off clock to avoid timing issues
	TCCR1B = 0x00;
		
	//Update values if needed
	if (_new_value_set){
		for (uint8_t i = 0; i < _count; i++){
			_values[i] = _new_values[i];
		}
		_new_value_set = 0;
	}
	if (_new_period){
		OCR1A = PWM_US_TO_CLICKS(_new_period, _prescaler);
		_new_period = 0;
	}
	
	//Set pins high
	for (uint8_t i = 0; i < _count; i++){	
		*_ports[i] |= _BV(_pins[i]);
	}

	//Trigger OCR1B to interrupt 'Real Soon Now'.  Set this as low as possible while still
	// retailing stability.  5 seems to be about the lowest number, we leave it at 10
	// to be sure.
	OCR1B = 0x2;
	
	//Reset counter
	TCNT1 = 0;
	
	//Re-enable clock
	TCCR1B |= _prescaler_mask;
}


/* 
 * The phase comparison.  When it overflows, we find the next highest value.
 */
ISR(TIMER1_COMPB_vect){
	//Turn off clock to avoid timing issues
	TCCR1B = 0x00;
	
	//Find the next value to trigger overflow
	uint16_t value = 0xFFFF;
	for (uint8_t i = 0; i < _count; i++){
		//Have we already passed the end of this phase?
		if (_values[i] <= TCNT1){
			*_ports[i] &= ~_BV(_pins[i]); //Turn off matching pins
		}
		//If not, is it the next lowest available value?
		else if (_values[i] < value){
			value = _values[i]; //Find next match
		}
	}
	
	//Set the timer for the next lowest value
	OCR1B = value;
	if (OCR1B <= TCNT1 + 1) OCR1B = TCNT1;

	//Re-enable clock
	TCCR1B |= _prescaler_mask;	
}
