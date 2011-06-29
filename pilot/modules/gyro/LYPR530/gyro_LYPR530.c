/*
 * LYPR530 implementation 
 */
#include "../gyro.h"
#include <util/delay.h>
#include <stdlib.h>
#include "../../../lib/analog/analog.h"

//The calibrated values show what the zero'd value of all axis should be.
// In theory this is 382, but it should be calibrated to find the actual value.
static uint16_t calibrated_values[2];

//Array used for raw analog reading
static uint16_t results[3];

void gyro_init(){
	//Send HP reset for a few ms
	*(&PORT_GYRO_HP_RESET - 0x1) |= _BV(PIN_GYRO_HP_RESET);
	PORT_GYRO_HP_RESET |= _BV(PIN_GYRO_HP_RESET);
	_delay_ms(30);
	PORT_GYRO_HP_RESET &= ~_BV(PIN_GYRO_HP_RESET);
	
	uint8_t calibration_data[4];
	uint8_t length = persist_read(PERSIST_SECTION_GYRO, calibration_data, 4);
	if (length == 4) {
		calibrated_values[0] = ((uint16_t) calibration_data[0] << 8) + calibration_data[1];
		calibrated_values[1] = ((uint16_t) calibration_data[2] << 8) + calibration_data[3];
		// calibrated_values[2] = ((uint16_t) calibration_data[4] << 8) + calibration_data[5];
	} else {
		status_error(STATUS_ERR_GYRO_CHECKSUM);
		_delay_ms(1000);
		status_error(0x00);

		//In theory the calibrated value is 382 (1.23v gyro input * 1024 / 3.3v vref).
		calibrated_values[0] = 382;
		calibrated_values[1] = 382;
		// calibrated_values[2] = 382;
	}

	uint8_t pins[3];
	pins[0] = PIN_GYRO_X;
	pins[1] = PIN_GYRO_Y;
	pins[2] = ADC_BATTERY;  //We init the battery check here too, since doing analog_init twice will kill first init settings
			//If you adjust these indices, please change the read index in battery.c module as well.
	analog_init(pins, 3, ANALOG_AREF);
}

double _gyro_raw_to_rad(uint16_t raw, uint16_t calibrated_zero){
	//Here we assume that we are using the 4x amplified output.  This gives us a sensitivity
	// of 3.33mV / degree / second.  (non-amplified output has sensitivity of 0.83).  Given
	// that a change of 1 unit in the ADC output theoretically reflects a change of 
	// 3.2227 mV (10 bits with a reference voltage of 3.3v), we can say that each single
	// 1 unit change in ADC output translates to just under 1 degree.  We can then say that
	// radians ~= (raw - calibrated) * (π/180)  (we round the actual value of π/180 up 
	// slightly to compensate for 1 unit being slightly less than one degree).
	return 0.01746 * (((int16_t) raw) - (((int16_t) calibrated_zero)));
}

vector_t gyro_get() {
    vector_t result;
    
    analog_read_a(results);
    
    result.x = _gyro_raw_to_rad(results[0], calibrated_values[0]);
    result.y = _gyro_raw_to_rad(results[1], calibrated_values[1]);
    // result.z = _gyro_raw_to_rad(results[2], calibrated_values[2]);
    
    return result;
}

void _gyro_read_multiple(uint32_t avg[2]){
	avg[0] = 0; avg[1] = 0; //avg[2] = 0;
    for (uint8_t i = 0; i < 64; i++){
	    analog_read_a(results);
	    
	    avg[0] += results[0];
	    avg[1] += results[1];
	    // avg[2] += results[2];

		//Give the ADC time to get some new values... if we use synchronous analog
		// library then we can get rid of this, otherwise just do it.  Calibration 
		//will still take less than a second anyway, so it's no big deal.
		_delay_ms(2);
	}

	//Divide by 64 for the average
	avg[0] = avg[0] >> 6;
	avg[1] = avg[1] >> 6;
	// avg[2] = avg[2] >> 6;
}

void gyro_calibrate(){
	uint32_t avg[2];
	_gyro_read_multiple(avg);
	uint8_t loop = 0;
	
	//At most, loop for 5 iterations, or until the difference is minimal.
	while (((abs((int16_t) results[0] - (int16_t) calibrated_values[0]) > 2) 
			|| (abs((int16_t) results[1] - (int16_t) calibrated_values[1]) > 2))
			// || (abs((int16_t) results[2] - (int16_t) calibrated_values[2]) > 2)) 
		&& (loop < 5)) {

		_gyro_read_multiple(avg);

		//Divide by 64 for the average
		calibrated_values[0] = avg[0];
		calibrated_values[1] = avg[1];
		// calibrated_values[2] = avg[2];
	
		_gyro_read_multiple(avg);
		
		loop++;
	}
	
	uint8_t calibration_data[6];
	calibration_data[0] = (uint8_t) (calibrated_values[0] >> 8);
	calibration_data[1] = (uint8_t) calibrated_values[0];
	calibration_data[2] = (uint8_t) (calibrated_values[1] >> 8);
	calibration_data[3] = (uint8_t) calibrated_values[1];
	// calibration_data[4] = (uint8_t) (calibrated_values[2] >> 8);
	// calibration_data[5] = (uint8_t) calibrated_values[2];
	persist_write(PERSIST_SECTION_GYRO, calibration_data, 4);
}
