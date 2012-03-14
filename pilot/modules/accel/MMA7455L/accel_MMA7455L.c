#include "../accel.h"
#include <util/delay.h>
#include <stdlib.h>
#include "../../../lib/i2c/i2c_master.h"

/*
 * MMA7455L implementation 
 */
#define ADDRESS 0x1D

#define AVERAGE_SAMPLE_SIZE 0x8

//Running average of values
static uint8_t running_average[AVERAGE_SAMPLE_SIZE][3]; //x, y, z as last index
static uint8_t running_average_pointer = 0;

static uint8_t message[8]; //Temporary array used for i2c communications

static vector_t result; //Accel values passed back

void accel_init(){
	//Init i2c
	i2c_master_init(400);
	
	//We *must* start interrupts before any i2c stuff will work, regardless of 
	// anything else.  If it matters, you should start this accel module after
	// other interrupt-sensitive modules.
	sei();

	//Enable accelerometer in 2g measurement mode
	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x16;
	message[2] = _BV(6) | _BV(2) | _BV(0); //6 == Disable external ready pin, 2 == 2g, 0 == measurement mode
	i2c_start_transceiver_with_data(message, 3);

	//Turn on 125Hz Filtering
	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x18;
	message[2] = _BV(7);
	i2c_start_transceiver_with_data(message, 3);

	// Reset the offset drift to zero; this can be re-calibrated later.
	// We read the last calibration from EEPROM; probably won't be 
	// perfect, but it will be better than 0x00 for everything.
	uint8_t calibration_data[6];
	uint8_t length = persist_read(PERSIST_SECTION_ACCEL, calibration_data, 6);
	if (length == 6) {
		message[0] = ADDRESS << 1 | I2C_WRITE;
		message[1] = 0x10;
		message[2] = calibration_data[0];
		message[3] = calibration_data[1];
		message[4] = calibration_data[2];
		message[5] = calibration_data[3];
		message[6] = calibration_data[4];
		message[7] = calibration_data[5];
		i2c_start_transceiver_with_data(message, 8);
	} else {
		status_error(STATUS_ERR_ACCEL_CHECKSUM);
		_delay_ms(1000);
		status_error(0x00);
	}

	//Read in AVERAGE_SAMPLE_SIZE readings so that our initial average is sane.
	for (uint8_t i = 0; i < AVERAGE_SAMPLE_SIZE; i++){
		accel_get(message);
	}	
}

/*
 * Checks if the accelerometer has data ready for reading yet.  Returns 1 if ready, 0 otherwise.
 */
uint8_t _accel_data_is_ready(){
	//Check if data is ready
	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x09; //Reset register pointer to 0x09 (Status)
	i2c_start_transceiver_with_data(message, 2);

	message[0] = ADDRESS << 1 | I2C_READ;
	i2c_start_transceiver_with_data(message, 2);
	i2c_get_data_from_transceiver(message, 2);	
	
	return message[1] & 0x1;
}

/*
 * Reads the data from the accelerometer, and copies it into the given buffer.
 */
void _accel_do_read(uint8_t *data){
	while (!_accel_data_is_ready());

	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x06; //Reset register pointer to 0x06 (8bit X)
	i2c_start_transceiver_with_data(message, 2);

	message[0] = ADDRESS << 1 | I2C_READ;
	i2c_start_transceiver_with_data(message, 4);
	i2c_get_data_from_transceiver(message, 4);
	
	data[0] = message[1];
	data[1] = message[2];
	data[2] = message[3];	
}

void _accel_read_multiple(int16_t avg[3]){
	uint8_t data[3];
	avg[0] = 0; avg[1] = 0; avg[2] = 0;
	for (uint8_t i = 0; i < 64; i++) {
		_accel_do_read(data);
		avg[0] += (int8_t) data[0];
		avg[1] += (int8_t) data[1];
		avg[2] += (int8_t) data[2];
	}
	// divide by 64
	avg[0] = avg[0] >> 6;
	avg[1] = avg[1] >> 6;
	avg[2] = avg[2] >> 6;
}

void accel_calibrate(){
	int16_t avg[3];
	int16_t offset[3];
	uint8_t loop = 0;
	
	offset[0] = 0; offset[1] = 0; offset[2] = 0;
	
	_accel_read_multiple(avg);
	
	//At most, loop for 20 iterations
	while ( ((abs(avg[0]) > 2) || (abs(avg[1]) > 2) || (abs(avg[2] - 64) > 2)) && (loop < 20)) {
		
		//Each offset value is calculated as (DESIRED_VALUE - ACTUAL_VALUE) * MULT_FACTOR
		// where DESIRED_VALUE is the theoretical reading (assuming the device is flat,
		// X and Y are 0x00 and Z is 0x3F).  Actual value is the average of a number of
		// readings (to eliminate spurious results), and the multiplication factor is
		// 2 or possibly a 'bit more'.  See Freescale Application Note AN3745 for details.
		offset[0] += (avg[0] * -2);  //Equivalent to ((0 - x) * 2)
		offset[1] += (avg[1] * -2);
		offset[2] += ((0x3F - avg[2]) * 2);
		
		// write compensation values into offset drift register ($10-$15)
		message[0] = ADDRESS << 1 | I2C_WRITE;
		message[1] = 0x10;
		message[2] = (uint8_t) offset[0]; 			// x LSB
		message[3] = (uint8_t) (offset[0] >> 8);	// x MSB
		message[4] = (uint8_t) offset[1]; 			// y LSB
		message[5] = (uint8_t) (offset[1] >> 8);	// y MSB
		message[6] = (uint8_t) offset[2];			// z LSB
		message[7] = (uint8_t) (offset[2] >> 8);	// z MSB
		
		i2c_start_transceiver_with_data(message, 8);

		//Give the (async) i2c some time to finish
		_delay_ms(20); 

		_accel_read_multiple(avg);
		
		loop++;
	}

	//Store calibration bytes to EEPROM
	uint8_t calibration_data[6];
	calibration_data[0] = (uint8_t) offset[0]; 			// x LSB
	calibration_data[1] = (uint8_t) ((uint16_t) offset[0] >> 8);	// x MSB
	calibration_data[2] = (uint8_t) offset[1]; 			// y LSB
	calibration_data[3] = (uint8_t) ((uint16_t) offset[1] >> 8);	// y MSB
	calibration_data[4] = (uint8_t) offset[2];			// z LSB
	calibration_data[5] = (uint8_t) ((uint16_t) offset[2] >> 8);	// z MSB
	
	persist_write(PERSIST_SECTION_ACCEL, calibration_data, 6);
}

vector_t accel_get() {
	//Don't wait around if the data is not ready...
	if (!_accel_data_is_ready()) return result;
	
	_accel_do_read(running_average[running_average_pointer]);
	running_average_pointer = (running_average_pointer + 1) % AVERAGE_SAMPLE_SIZE;

	int16_t totals[3] = {0};
	for (uint8_t i = 0; i < AVERAGE_SAMPLE_SIZE; i++){
		for (uint8_t j = 0; j < 3; j++){
			totals[j] += ((int8_t) running_average[i][j]);
		}
	}

	//Calculate the actual g values; for 2g sensitivity (which we are using), 
	// there are 64 intervals per g value.
	result.x = (double) totals[0] / AVERAGE_SAMPLE_SIZE / 0x3F;
	result.y = (double) totals[1] / AVERAGE_SAMPLE_SIZE / 0x3F;
	result.z = (double) totals[2] / AVERAGE_SAMPLE_SIZE / 0x3F;
	
	return result;
}
