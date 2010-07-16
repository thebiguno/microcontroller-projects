#include "pwm.h"

//Macro to convert VALUE (in µs) to a clock tick count.
#define PWM_US_TO_CLICKS(VALUE) F_CPU /1000000 * VALUE / 256

//Commands to turn clock on and off.
#define PWM_CLOCK_ON TCCR1B = _BV(CS12)
#define PWM_CLOCK_OFF TCCR1B = 0x00

//Current values are here
uint16_t _values[PWM_MAX_PINS] = {0,0,0,0,0,0,0,0};

//New values are here, they are copied over at the start of a new waveform
uint16_t _new_values[PWM_MAX_PINS] = {0,0,0,0,0,0,0,0};
uint8_t _new_value_set = 0; //Set to true when updated values

//Variables used to store config data
volatile uint8_t *_ports[PWM_MAX_PINS];		//Array of ports used
volatile uint8_t *_ddrs[PWM_MAX_PINS];		//Array of DDRs used
uint8_t _pins[PWM_MAX_PINS];				//Array of pins used
uint8_t _count;								//How many pins should be used



void pwm_init(volatile uint8_t *ports[],
				volatile uint8_t *ddrs[],
				uint8_t pins[],
				uint8_t count,
				uint16_t period) {
			
	//Store values
	*_ports = *ports;
	*_ddrs = *ddrs;
	*_pins = *pins;
	_count = (count <= PWM_MAX_PINS ? count : PWM_MAX_PINS);
				
	//Enable all pins for output
	for (uint8_t i = 0; i < _count; i++){
		*_ddrs[i] |= _BV(_pins[i]);
	}
				
	//Set up the timer to run at F_CPU / 256 (prescaler), in normal mode.  You can 
	// change the prescaler to get a different frequency range, but make sure you 
	// adjust the PWM_US_TO_CLICKS macro as well.
	TCCR1A = 0x0;
	PWM_CLOCK_ON;
	
	//OCR1A controls the PWM period
	OCR1A = PWM_US_TO_CLICKS(period);
	//OCR1B controls the PWM phase.  It is initialized later.
	
	//Enable compare interrupt on both channels
	TIMSK1 = _BV(OCIE1A) | _BV(OCIE1B);
	
	//Enable interrupts
	sei();
	
	//Force interrupt on compare A initially
	TCCR1C |= _BV(FOC1A);	
}

/*
 * Sets the phase of the pin at the given index to the specified value.
 */
void pwm_set(uint8_t index, uint16_t phase){
	if (index < _count){
		_new_values[index] = phase;
		_new_value_set = 1;
	}
}

/* 
 * The frequency comparison.  When it overflows, we reset the timer to 0.
 */
ISR(TIMER1_COMPA_vect){
	//Turn off clock to avoid timing issues
	PWM_CLOCK_OFF;
	
	//Update values if needed
	if (_new_value_set){
		for (uint8_t i = 0; i < _count; i++){
			_values[i] = _new_values[i];
		}
		_new_value_set = 0;
	}
	
	//Set pins high
	for (uint8_t i = 0; i < _count; i++){
		*_ports[i] |= _BV(_pins[i]);
	}

	//Trigger OCR1B to interrupt 'Real Soon Now'.  Set this as low as possible while still
	// retailing stability.  5 seems to be about the lowest number, we leave it at 10
	// to be sure.
	OCR1B = 0xA;
	
	//Reset counter
	TCNT1 = 0;
	
	//Re-enable clock
	PWM_CLOCK_ON;
}


/* 
 * The phase comparison.  When it overflows, we find the next highest value.
 */
ISR(TIMER1_COMPB_vect){
	//Turn off clock to avoid timing issues
	PWM_CLOCK_OFF;
	
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
	PWM_CLOCK_ON;	
}
