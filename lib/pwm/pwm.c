//TODO We should probably time how long it takes to run the timer interrupts, and compensate
// for the lag.  This will mostly affect high frequency PWM, and will manifest itself 
// in the form of slightly lower frequencies than you would expect.  (From experiments,
// a 2ms period (which should be 500Hz) registers as 485Hz).

#include "pwm.h"

//Macro to convert VALUE (in µs) to a clock tick count with a specified prescaler.
#define PWM_US_TO_CLICKS(value, prescaler) (F_CPU / 1000000) * (value / prescaler)

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
		_ports[i] = ports[i];
		_ddrs[i] = ports[i] - 0x1;
		_pins[i] = pins[i];
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
	
	//Initialize _values and _new_values to 0, and enable all pins for output
	for (uint8_t i = 0; i < _count; i++){
		*_ddrs[i] |= _BV(_pins[i]);
		_values[i] = 0;
		_new_values[i] = 0;
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
		*_ports[i] &= ~_BV(_pins[i]);
	}	
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
			_values[i] = _new_values[i];
		}
		_new_value_set = 0;
	}
	if (_new_period){
		OCRA = PWM_US_TO_CLICKS(_new_period, _prescaler);
		_new_period = 0;
	}
	
	//Set pins high, if the PWM value is > 0.
	for (uint8_t i = 0; i < _count; i++){
		if (_values[i] > 0){
			*_ports[i] |= _BV(_pins[i]);
		}
	}

	//Trigger OCRB to interrupt 'Real Soon Now'.  Set this as low as possible while still
	// retailing stability.  5 seems to be about the lowest number, we leave it at 10
	// to be sure.
	OCRB = 0xA;
	
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
	
	//Find the next value to trigger overflow
	uint16_t value = 0xFFFF;
	for (uint8_t i = 0; i < _count; i++){
		//Have we already passed the end of this phase?
		if (_values[i] <= TCNT){
			*_ports[i] &= ~_BV(_pins[i]); //Turn off matching pins
		}
		//If not, is it the next lowest available value?
		else if (_values[i] < value){
			value = _values[i]; //Find next match
		}
	}
	
	//Set the timer for the next lowest value
	OCRB = value;
	if (OCRB <= TCNT + 1) OCRB = TCNT;

	//Re-enable clock
	TCCRB |= _prescaler_mask;
}
