//TODO We should probably time how long it takes to run the timer interrupts, and compensate
// for the lag.  This will mostly affect high frequency PWM, and will manifest itself 
// in the form of slightly lower frequencies than you would expect.  (From experiments,
// a 2ms period (which should be 500Hz) registers as 485Hz).

#include "pwm.h"

struct pwm_pin_t {
	volatile uint8_t *port;
	uint8_t pin;
	
	uint16_t value;
	uint16_t next_value;
};

//Macro to convert VALUE (in µs) to a clock tick count with a specified prescaler.
#define PWM_US_TO_CLICKS(value, prescaler) (F_CPU / 1000000) * (value / prescaler)

//New value flags; they are checked at the start of a new waveform
static uint8_t _new_value_set = 0; //Set to true when updated values
static uint32_t _new_period = 0; //New period defined, changed in COMPA interrupt

//Variables used to store config data
static struct pwm_pin_t _pins[PWM_MAX_PINS];		//Array of pins
static uint8_t _count;								//How many pins should be used
static volatile uint8_t _next_pin;					//Index of the next pin to fire

static uint16_t _prescaler = 0x0;
static uint8_t _prescaler_mask = 0x0;

//Figure out which registers to use, depending on the chip in use
#if defined(__AVR_ATtiny25__)   || \
	defined(__AVR_ATtiny45__)   || \
	defined(__AVR_ATtiny85__)

#define PWM_8_BIT
#define TCCRA			TCCR0A
#define TCCRB			TCCR0B
#define OCRA			OCR0A
#define OCRB			OCR0B
#define TIMSKR			TIMSK
#define OCIEA			OCIE0A
#define OCIEB			OCIE0B
#define FIR				TIFR	//Force Interrupt Register
#define FOCA			OCF0A
#define TCNT			TCNT0

#elif defined(__AVR_ATmega168__)   || \
	defined(__AVR_ATmega328__)     || \
	defined(__AVR_ATmega328P__)    || \
	defined(__AVR_ATmega324P__)    || \
	defined(__AVR_ATmega644__)     || \
	defined(__AVR_ATmega644P__)    || \
	defined(__AVR_ATmega644PA__)   || \
	defined(__AVR_ATmega1284P__)

#define TCCRA			TCCR1A
#define TCCRB			TCCR1B
#define OCRA			OCR1A
#define OCRB			OCR1B
#define TIMSKR 			TIMSK1
#define OCIEA			OCIE1A
#define OCIEB			OCIE1B
#define FIR				TCCR1C
#define FOCA			FOC1A
#define TCNT			TCNT1

#else
	#error You must confirm PWM setup for your chip!  Please verify that MMCU is set correctly, and that there is a matching check definition in pwm.h
#endif


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
		_pins[i].port = ports[i];
		_pins[i].pin = pins[i];
		*(_pins[i].port - 0x1) |= _BV(_pins[i].pin);
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
				
	TCCRA = 0x00;
	TCCRB |= _prescaler_mask;
	
	//OCRA controls the PWM period
	OCRA = PWM_US_TO_CLICKS(period, _prescaler);
	//OCRB controls the PWM phase.  It is initialized later.
	
	//Enable compare interrupt on both channels
	TIMSKR = _BV(OCIEA) | _BV(OCIEB);
	
	//Enable interrupts if the NO_INTERRUPT_ENABLE define is not set.  If it is, you need to call sei() elsewhere.
#ifndef NO_INTERRUPT_ENABLE
	sei();
#endif

	//Force interrupt on compare A initially
	FIR |= _BV(FOCA);
}

void pwm_start(){
	TCNT = 0x00;	//Restart timer counter
	TIMSKR = _BV(OCIEA) | _BV(OCIEB);	//Enable output compare match interrupt enable
	TCCRB |= _prescaler_mask;	//Enable 
}

void pwm_stop(){
	TCCRB = 0x00;
	TIMSKR |= _BV(OCIEA) | _BV(OCIEB);

	//Set pins low
	for (uint8_t i = 0; i < _count; i++){
		*(_pins[i].port) &= ~_BV(_pins[i].pin);
	}
}

void pwm_set_phase(uint8_t index, uint16_t phase){
	if (index < _count){
		_pins[index].next_value = PWM_US_TO_CLICKS(phase, _prescaler);
		_new_value_set = 1;
	}
}

void pwm_set_period(uint32_t period){
	_new_period = period;
}


//The comparison method used in qsort (in the COMPA vector)
static int16_t _compare_values(const void *pin1, const void *pin2){
	 const struct pwm_pin_t *pwm1 = pin1;
	 const struct pwm_pin_t *pwm2 = pin2;
	 return pwm1->value - pwm2->value;
}

/* 
 * The frequency comparison.  When it overflows, we reset the timer to 0.
 */
#ifdef PWM_8_BIT
EMPTY_INTERRUPT(TIM0_OVF_vect)
ISR(TIM0_COMPA_vect){
#else
ISR(TIMER1_COMPA_vect){
#endif
	//Turn off clock to avoid timing issues
	TCCRB = 0x00;
	
	//Update values if needed
	if (_new_value_set){
		for (uint8_t i = 0; i < _count; i++){
			_pins[i].value = _pins[i].next_value;
		}
		
		//Sort the _pins array by value.  This speeds up each successive compare operation in COMPB.
		qsort(_pins, _count, sizeof _pins[0], _compare_values);
		
		_new_value_set = 0;
	}
	if (_new_period){
		OCRA = PWM_US_TO_CLICKS(_new_period, _prescaler);
		_new_period = 0;
	}
	
	//Set pins high, if the PWM value is > 0.
	//NOTE: If you need high speed for many pins, change this loop to hardcoded PORTX assignments.
	for (uint8_t i = 0; i < _count; i++){
		if (_pins[i].value > 0){
			*(_pins[i].port) |= _BV(_pins[i].pin);
		}
	}
	
	//Reset next pin pointer to first index in the sorted array
	_next_pin = 0;

	//Set to the first compare value
	OCRB = _pins[0].next_value;
	
	//Reset counter
	TCNT = 0;
	
	//Re-enable clock
	TCCRB |= _prescaler_mask;
}


/* 
 * The phase comparison.  When it overflows, we find the next highest value.
 */
#ifdef PWM_8_BIT
ISR(TIM0_COMPB_vect){
#else
ISR(TIMER1_COMPB_vect){
#endif

	//Turn off clock to avoid timing issues
	TCCRB = 0x00;
	
	//Turn off each pin starting at _next_pin and ending at the pin whose value is greater than _pin[_next_pin].value
	while (_pins[_next_pin].value <= TCNT){
		*(_pins[_next_pin].port) &= ~_BV(_pins[_next_pin].pin); //Turn off pin
		_next_pin++;
	}
	
	//Set the timer for the next lowest value if available; 0xFFFF otherwise
	if (_next_pin < _count){
		OCRB = _pins[_next_pin].value;
	}
	else {
		OCRB = 0xFFFF;
	}

	//Re-enable clock
	TCCRB |= _prescaler_mask;
}
