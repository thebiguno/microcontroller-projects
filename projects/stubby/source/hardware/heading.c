#include "heading.h"

#define SAMPLE_SIZE 8

static volatile double samples[SAMPLE_SIZE];
static volatile double filtered;
static volatile uint8_t sample_count;

void heading_init(){
	magnetometer_init();
	_delay_ms(100);
	filtered = magnetometer_read_heading();
	for (uint8_t i = 0; i < SAMPLE_SIZE; i++){
		samples[i] = filtered;
	}
	
	//Set up the timer to run at F_CPU / 1024, in normal mode
	TCCR2A = 0x0;
	TCCR2B |= _BV(CS22) | _BV(CS21) | _BV(CS20);
	TIMSK2 = _BV(TOIE2);
}

double heading_read(){
	return filtered;
}

void heading_take_reading(){
	sample_count = (sample_count + 1) & 0x07;	//Modulus addition for sample size 8
	samples[sample_count] = magnetometer_read_heading();
	double f = samples[0];
	
	//We assume that all samples are in the same general range.  We check the first
	// sample; if it is in the east / south / west hemisphere, we rotate all samples
	// by PI (180 degrees).  We then rotate the average back when finished.  This 
	// will prevent the wraparound from breaking the averages.  This procedure
	// would fall apart if the 8 samples are not all 'close enough' to each other.
	uint8_t rotate = (f >= (M_PI / 2) || f <= (M_PI / -2)) ? 1 : 0;
	
	if (rotate){
		f += M_PI;
		for (uint8_t i = 1; i < SAMPLE_SIZE; i++){
			f += normalize_angle(samples[i] + M_PI);
		}
		f = normalize_angle((f / SAMPLE_SIZE) - M_PI);
	}
	else {
		for (uint8_t i = 1; i < SAMPLE_SIZE; i++){
			f += samples[i];
		}
		f = f / SAMPLE_SIZE;
	}
	
	filtered = f;
}

EMPTY_INTERRUPT(TIMER2_COMPA_vect)
EMPTY_INTERRUPT(TIMER2_COMPB_vect)
ISR(TIMER2_OVF_vect){
	sei();
	static uint8_t i = 0;
	i++;
	if (i >= 5){
		i = 0;
		heading_take_reading();
	}
}
