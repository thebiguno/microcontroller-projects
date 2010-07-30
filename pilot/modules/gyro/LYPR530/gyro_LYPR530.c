/*
 * LYPR530 implementation 
 */
#include "../gyro.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../../../lib/analog/analog.h"

#define CALIBRATION_SAMPLE_SIZE 0x20

//The calibrated values show what the zero'd value of all axis should be.
// In theory this is 382, but it should be calibrated to find the actual value.
static uint16_t calibrated_values[3];

void gyro_init(){
	//Send HP reset for a few ms
	DDRC |= _BV(PIN3);
	PORTC |= _BV(PIN3);
	_delay_ms(10);
	PORTC &= ~_BV(PIN3);
	DDRC &= ~_BV(PIN3);

	//In theory the calibrated value is 382 (1.23v gyro input * 1024 / 3.3v vref).
	// We start with this as a default, and reset it as needed.
	calibrated_values[0] = 382;
	calibrated_values[1] = 382;
	calibrated_values[2] = 382;

	uint8_t pins[3];
	pins[0] = 0;
	pins[1] = 1;
	pins[2] = 2;
	analog_init(pins, 3);
}

double _gyro_raw_to_rad(uint16_t raw, uint16_t calibrated_zero){
	//Here we assume that we are using the 4x amplified output.  This gives us a sensitivity
	// of 3.33mV / degree / second.  (non-amplified output has sensitivity of 0.83).  Given
	// that a change of 1 unit in the ADC output theoretically reflects a change of 
	// 3.2227 mV (10 bits with a reference voltage of 3.3v), we can say that each single
	// 1 unit change in ADC output translates to just under 1 degree.  We can then say that
	// radians ~= (raw - calibrated) * (π/180)  (we round the actual value of π/180 up 
	// slightly to compensate for 1 unit being slightly less than one degree).
	return 0.01746 * (raw - calibrated_zero);
}

vector_t gyro_get() {
    vector_t result;
    
    uint16_t results[3];
    analog_read_a(results);
    
    result.x = _gyro_raw_to_rad(results[0], calibrated_values[0]);
    result.y = _gyro_raw_to_rad(results[1], calibrated_values[1]);
    result.z = _gyro_raw_to_rad(results[2], calibrated_values[2]);
    
    return result;
}

void gyro_calibrate(){
	for (int i = 0; i < 3; i++){
		uint32_t total = 0;
		for (int j = 1; j < CALIBRATION_SAMPLE_SIZE; j++){
			total = total + analog_read_p(i);
			
			//Give the ADC time to get some new values... if we use synchronous analog
			// library then we can get rid of this, otherwise just do it.  Calibration 
			//will still take less than a second anyway, so it's no big deal.
			_delay_ms(1); 
		}
		calibrated_values[i] = total / (CALIBRATION_SAMPLE_SIZE);
	}	
}
