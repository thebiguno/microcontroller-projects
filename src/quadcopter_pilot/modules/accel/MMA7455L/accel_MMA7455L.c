#include "../accel.h"
#include "../../../../../lib/i2c/i2c_master.h"

/*
 * MMA7455L implementation 
 */
#define ADDRESS 0x1D

#define AVERAGE_SAMPLE_SIZE 0x20
#define CALIBRATION_SAMPLE_SIZE 0x80

//Running average of values
static uint8_t running_average_x[AVERAGE_SAMPLE_SIZE];
static uint8_t running_average_y[AVERAGE_SAMPLE_SIZE];
static uint8_t running_average_z[AVERAGE_SAMPLE_SIZE];
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

	//Reset the offset drift to zero; this can be calibrated later
	//TODO Read the last calibration from EEPROM; probably won't be 
	// perfect, but it will be better than 0x00 for everything.
	
	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x10;
	message[2] = 0x00;
	message[3] = 0x00;
	message[4] = 0x00;
	message[5] = 0x00;
	message[6] = 0x00;
	message[7] = 0x00;
	i2c_start_transceiver_with_data(message, 8);

	//Read in AVERAGE_SAMPLE_SIZE readings so that our initial average is sane.
	for (uint8_t i = 0; i < AVERAGE_SAMPLE_SIZE; i++){
		accel_get();
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

	
	//Send the calibration data, starting at register 0x10
	message[0] = ADDRESS << 1 | I2C_WRITE;
	message[1] = 0x10;
	message[2] = (total[0] / CALIBRATION_SAMPLE_SIZE) * -2;
	message[3] = 0x00;
	message[4] = (total[1] / CALIBRATION_SAMPLE_SIZE) * -2;
	message[5] = 0x00;
	message[6] = (64 - (total[2] / CALIBRATION_SAMPLE_SIZE)) * 2;
	message[7] = 0x00;
	i2c_start_transceiver_with_data(message, 8);

	//TODO Store calibration bytes to EEPROM
}

vector_t accel_get() {
	uint8_t data[3];
	_accel_do_read(data);

	running_average_x[running_average_pointer] = data[0];
	running_average_y[running_average_pointer] = data[1];
	running_average_z[running_average_pointer] = data[2];

	running_average_pointer = (running_average_pointer + 1) % AVERAGE_SAMPLE_SIZE;

	vector_t result;
	result.x = 0;
	result.y = 0;
	result.z = 0;
	for (uint8_t i = 0; i < AVERAGE_SAMPLE_SIZE; i++){
		result.x += ((int8_t) running_average_x[i] / 64.0);
		result.y += ((int8_t) running_average_y[i] / 64.0);
		result.z += ((int8_t) running_average_z[i] / 64.0);
	}

	result.x = result.x / AVERAGE_SAMPLE_SIZE;
	result.y = result.y / AVERAGE_SAMPLE_SIZE;
	result.z = result.z / AVERAGE_SAMPLE_SIZE;
	
	return result;
}
