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
	i2c_master_init(100);

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
	int16_t total[3] = {0};
	uint8_t data[3];
	for (uint8_t i = 0; i < CALIBRATION_SAMPLE_SIZE; i++){
		_accel_do_read(data);
		
		total[0] += (int8_t) data[0];
		total[1] += (int8_t) data[1];
		total[2] += (int8_t) data[2];
	}

	//Each offset value is calculated as (DESIRED_VALUE - ACTUAL_VALUE) * MULT_FACTOR
	// where DESIRED_VALUE is the theoretical reading (assuming the device is flat,
	// X and Y are 0x00 and Z is 0x3F).  Actual value is the average of a number of
	// readings (to eliminate spurious results), and the multiplication factor is
	// 2 (or possibly a 'bit more' -- see Freescale Application Note AN3745).
	//Send the calibration data, starting at register 0x10
	//TODO: This calibration routine needs to be improved.  First, we currently have
	// no allowance for negative values.  Second, it is recommended in AN3745 to
	// use an iterative process to find the actual values, as the first time around
	// may not be completely accurate.
	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x10;
	message[2] = ((0x00 - (total[0] / CALIBRATION_SAMPLE_SIZE)) * 2);
	message[3] = 0x00;
	message[4] = ((0x00 - (total[1] / CALIBRATION_SAMPLE_SIZE)) * 2);
	message[5] = 0x00;
	message[6] = ((0x3F - (total[2] / CALIBRATION_SAMPLE_SIZE)) * 3);
	message[7] = 0x00;
	i2c_start_transceiver_with_data(message, 8);

	//Store calibration bytes to EEPROM
	uint8_t calibration_data[6];
	calibration_data[0] = message[2];
	calibration_data[1] = message[3];
	calibration_data[2] = message[4];
	calibration_data[3] = message[4];
	calibration_data[4] = message[6];
	calibration_data[5] = message[7];
	persist_write(PERSIST_SECTION_ACCEL, PERSIST_OFFSET, calibration_data, 6);
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
