#include <stdlib.h>
#include "../accel.h"
#include "../../../../../lib/i2c/i2c_master.h"

/*
 * MMA7455L implementation 
 */
#define ADDRESS 0x1D

#define AVERAGE_SAMPLE_SIZE 0x10
#define CALIBRATION_SAMPLE_SIZE 0xF0

#define PERSIST_OFFSET 0x00  //The address to store offset calibration data

//Running average of values
static uint8_t running_average[AVERAGE_SAMPLE_SIZE][3]; //x, y, z as last index
static uint8_t running_average_pointer = 0;

uint8_t message[8]; //Temporary array used for i2c communications

void accel_init(){
	//Init i2c
	i2c_master_init(400);

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

	//Reset the offset drift to zero; this can be re-calibrated later.
	//We read the last calibration from EEPROM; probably won't be 
	// perfect, but it will be better than 0x00 for everything.
	
	uint8_t calibration_data[6];
	persist_read(PERSIST_SECTION_ACCEL, PERSIST_OFFSET, calibration_data, 6);
	
	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x10;
	message[2] = calibration_data[0];
	message[3] = calibration_data[1];
	message[4] = calibration_data[2];
	message[5] = calibration_data[3];
	message[6] = calibration_data[4];
	message[7] = calibration_data[5];
	i2c_start_transceiver_with_data(message, 8);

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

void accel_calibrate(){
	uint8_t data[3];
	int16_t avg[3];
	int16_t offset[3];
	uint8_t loop = 0;
	int16_t x, y, z;
	offset[0] = 0; offset[1] = 0; offset[2] = 0;
	
	char temp[32];
	
	do {
		// read the axes multiple times and average
		avg[0] = 0; avg[1] = 0; avg[2] = 0;
		for (uint8_t i = 0; i < 64; i++) {
			_accel_do_read(data);
			sprintf(temp, "rx=%x, ry=%x, rz=%x\n\r", data[0], data[1], data[2]);
			serial_write_s(temp);
            avg[0] += (int8_t) data[0];
            avg[1] += (int8_t) data[1];
            avg[2] += (int8_t) data[2];
		}
		sprintf(temp, "ax=%i, ay=%i, az=%i\n\r", avg[0], avg[1], avg[2]);
		serial_write_s(temp);
		// divide by 64
		x = avg[0] >> 6;
		y = avg[1] >> 6;
		z = avg[2] >> 6;
		offset[0] += (x * -2);
		offset[1] += (y * -2);
		offset[2] += ((64 - z) * 2);
		
		sprintf(temp, "ox=%i, oy=%i, oz=%i\n\r", offset[0], offset[1], offset[2]);
		serial_write_s(temp);
		
		// write compensation values into offset drift register ($10-$15)
		message[0] = ADDRESS << 1 | I2C_WRITE;
		message[1] = 0x10;
		message[2] = (uint8_t) offset[0]; // x LSB
		message[3] = (uint8_t) (offset[0] >> 8);      // x MSB
		message[4] = (uint8_t) offset[1]; // y LSB
		message[5] = (uint8_t) (offset[1] >> 8);      // y MSB
		message[6] = (uint8_t) offset[2]; // z LSB
		message[7] = (uint8_t) (offset[2] >> 8);      // z MSB
		i2c_start_transceiver_with_data(message, 8);
		
		// remeasure the values
		avg[0] = 0; avg[1] = 0; avg[2] = 0;
		for (uint8_t i = 0; i < 64; i++) {
			_accel_do_read(data);
            avg[0] += (int8_t) data[0];
            avg[1] += (int8_t) data[1];
			avg[2] += (int8_t) data[2];
		}
		// divide by 64
		x = avg[0] >> 6;
		y = avg[1] >> 6;
		z = avg[2] >> 6;
		loop++;
	} while ( ((abs(x) > 2) || (abs(y) > 2) || (abs(z - 64) > 2)) && (loop < 10));

	//Store calibration bytes to EEPROM
	uint8_t calibration_data[6];
	calibration_data[0] = message[2];
	calibration_data[1] = message[3];
	calibration_data[2] = message[4];
	calibration_data[3] = message[4];
	calibration_data[4] = message[6];
	calibration_data[5] = message[7];
//	persist_write(PERSIST_SECTION_ACCEL, PERSIST_OFFSET, calibration_data, 6);
}

vector_t accel_get() {
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
	vector_t result;
	result.x = (double) totals[0] / AVERAGE_SAMPLE_SIZE / 0x3F;
	result.y = (double) totals[1] / AVERAGE_SAMPLE_SIZE / 0x3F;
	result.z = (double) totals[2] / AVERAGE_SAMPLE_SIZE / 0x3F;
	
	return result;
}
