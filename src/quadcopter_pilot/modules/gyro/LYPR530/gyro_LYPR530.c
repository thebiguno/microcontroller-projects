/*
 * LYPR530 implementation 
 */
#include "../gyro.h"
#include <avr/interrupt.h>
#include <util/delay.h>

#define CALIBRATION_SAMPLE_SIZE 0x20

//The calibrated values show what the zero'd value of all axis should be.
// Calibration is started when you set calibrated_values_pointer to a number
// larger than 0; then, each iteration through the ISR it will record the value
// in the calibration array rather than the results array.
// When the ISR records the calibration value for index 1, it then averages all
// recorded values from [1..CALIBRATION_SAMPLE_SIZE - 1], and records the result
// in index 0.  This value is used to determine the rotation speed to result value
// mapping.
static uint16_t calibrated_values[CALIBRATION_SAMPLE_SIZE][3];
static int8_t calibrated_values_pointer = 0;

//The actual results.  This array is updated by ISR, and the actual values are obtained
// by request at gyro_get().
static uint16_t results[3];

//Counter for the ISR to know which ADC it is talking to at a given time.  Used to 
// configure the ADC mux channel section.
static uint8_t adc_mux = 0;

void gyro_init(){
	ADMUX = 0x00;

	//TODO configure ADPS2::0 (prescaler) to give speed / accuracy tradeoff
	ADCSRA |= _BV(ADEN) | _BV(ADIE) | 4;  	//ADC Enable, ADC Interrupt Enable

	ADCSRB = 0x00;	//Free running mode

	//Disable digital input on pins 0..2
	DIDR0 |= _BV(0) | _BV(1) | _BV(2);

	//Start ADC conversions
	ADCSRA |= _BV(ADSC);

	//Send HP reset for a few ms
	DDRC |= _BV(PIN3);
	PORTC |= _BV(PIN3);
	_delay_ms(10);
	PORTC &= ~_BV(PIN3);
	DDRC &= ~_BV(PIN3);

	//In theory the calibrated value is 382 (1.23v gyro input * 1024 / 3.3v vref).
	// We start with this as a default, and reset it as needed.
	calibrated_values[0][0] = 382;
	calibrated_values[0][1] = 382;
	calibrated_values[0][2] = 382;
	
	sei();
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
    
    result.x = _gyro_raw_to_rad(results[0], calibrated_values[0][0]);
    result.y = _gyro_raw_to_rad(results[1], calibrated_values[0][1]);
    result.z = _gyro_raw_to_rad(results[2], calibrated_values[0][2]);
    
    return result;
}

void gyro_calibrate(){
	//Mark that calibration needs to start... the calibration happens in the ISR,
	// and gyro_get() calls will still return without interruption.  The new
	// calibration results will take effect when calibration is complete.
	calibrated_values_pointer = CALIBRATION_SAMPLE_SIZE - 1;
}

char temp[32];
ISR(ADC_vect){
	//Read last ADC value assuming calibration is finished
	results[adc_mux] = ADC;
	if (calibrated_values_pointer > 0){
		calibrated_values[calibrated_values_pointer][adc_mux] = results[adc_mux];
		if (adc_mux >= 2) calibrated_values_pointer--;
		
		//If we are now finished calibration, average all the values into
		// index 0 in the calibrated_values array.
		if (calibrated_values_pointer == 0){
			uint32_t totals[3];
			
			for (int j = 0; j < 3; j++){
				for (int i = 1; i < CALIBRATION_SAMPLE_SIZE; i++){			
					totals[j] = totals[j] + calibrated_values[i][j];
				}
				calibrated_values[0][j] = totals[j] / (CALIBRATION_SAMPLE_SIZE - 1);
			}
		}
	}

	//Increment to next pin and start again
	adc_mux = (adc_mux + 1) % 3;
	ADMUX = ((ADMUX) & 0xF0) | adc_mux;

	//Start ADC again
	ADCSRA |= _BV(ADSC);
}

