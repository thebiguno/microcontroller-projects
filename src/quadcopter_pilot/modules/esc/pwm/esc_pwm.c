#include <avr/io.h>
#include <avr/interrupt.h>

#define PWM_CLOCK_BASE F_CPU / 1000000 //Period in 탎
#define PWM_FREQUENCY 20000		//20000탎 == 20ms == 50Hz

#define ESC_PORT PORTB
#define ESC_DDR DDRB
#define ESC_PIN_0 1
#define ESC_PIN_1 2
#define ESC_PIN_2 3
#define ESC_PIN_3 4

//Current values are here
static uint16_t esc_value[4] = {0xFF, 0x00, 0x00, 0x00};

//Store new values here, they are copied over at the start of a new waveform
static uint16_t esc_new_value[4] = {0x00, 0x00, 0x00, 0x00};

static uint8_t pins[4] = {ESC_PIN_0, ESC_PIN_1, ESC_PIN_2, ESC_PIN_3};

void esc_init() {
	//Set PWM pins into output mode
	ESC_DDR |= _BV(ESC_PIN_0) | _BV(ESC_PIN_1) | _BV(ESC_PIN_2) | _BV(ESC_PIN_3);

	//Set up the timer to run at F_CPU (/256 prescaler), in normal mode
	TCCR1A = 0x0;
	TCCR1B |= (1 << CS12);
	
	//Controls the frequency -- F_CPU / 1000 * PWM_FREQUENCY (i.e. PWM_FREQUENCY milliseconds)
	OCR1A = (uint16_t) (PWM_CLOCK_BASE * PWM_FREQUENCY / 256);
	
	//OCR1B will be used to generate events for each ESC... this is set in COMPA_vect
	
	//Enable compare interrupt on both channels
	TIMSK1 = (1 << OCIE1A) | (1 << OCIE1B);

	//Enable interrupts
	sei();
}

/*
 * 
 */
void esc_set(uint8_t armed, double speed[]) {
	/*
    if (armed) {
        // TODO
        for (int i = 0; i < 4; i++) {
            _esc_set(i, scale(speed[i]));
        }
    } else {
        for (int i = 0; i < 4; i++) {
            _esc_set(i, 0);
        }
    }
    */
}

/* 
 * The frequency comparison.  When it overflows, we reset the timer to 0.
 */
ISR(TIMER1_COMPA_vect){
	TCNT1 = 0;
	
	//Set all pins high
	ESC_PORT |= _BV(ESC_PIN_0) | _BV(ESC_PIN_1) | _BV(ESC_PIN_2) | _BV(ESC_PIN_3);

	//Trigger OCR1B to interrupt soon (if you don't get proper PWM, 
	// increase this.  200 clock cycles seems to be within reason; 
	// if you set it too soon, the clock will already have passed, 
	// and the output will be on the entire time.)
	//This gives you an effective low limit on the pulse width.  
	// TCNT1 + 250 works out to about 16탎; given that the lowest 
	// value for ESCs is going to be about 1000탎, this is perfectly 
	// fine.
	OCR1B = TCNT1 + 250;
}

/* 
 * The phase comparison.  When it overflows, we find the next highest value.
 */
ISR(TIMER1_COMPB_vect){
	

	//Find the next value to trigger overflow
	uint16_t value = 0xFFFF;
	for (uint8_t i = 0; i < 4; i++){
		//Have we already passed this phase?
		if (esc_value[i] <= OCR1B){
			ESC_PORT &= ~_BV(pins[i]); //Turn off matching pins
		}
		//If not, is it the next lowest?
		else if (esc_value[i] < value){
			value = esc_value[i]; //Find next match
		}
	}
	
	OCR1B = (uint16_t) (PWM_CLOCK_BASE * value / 256);
}
