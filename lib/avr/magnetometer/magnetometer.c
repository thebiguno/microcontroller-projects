#include "magnetometer.h"

#define MAGNETOMETER_ADDRESS 0x1E
#define ALPHA 0.2

static int16_t raw[3];
static int16_t offsets[3];

void magnetometer_init(){
	twi_init();
	
	uint8_t message[2];
	message[0] = 0x00;		//Register 0
	message[1] = 0x78;		//8 sample average, 75 Hz, normal mode
	twi_write_to(MAGNETOMETER_ADDRESS, message, 2, TWI_BLOCK, TWI_STOP);
	message[0] = 0x01;		//Register 1
	message[1] = 0x20;		//Gain 1 (± 1.3 Ga)
	twi_write_to(MAGNETOMETER_ADDRESS, message, 2, TWI_BLOCK, TWI_STOP);
	message[0] = 0x02;		//Register 2
	message[1] = 0x00;		//Comtinuous measurement mode
	twi_write_to(MAGNETOMETER_ADDRESS, message, 2, TWI_BLOCK, TWI_STOP);
	
	int16_t data[3];
	magnetometer_read_raw(data);	//Disregard first reading after changing mode
}

void magnetometer_set_offsets(int16_t x, int16_t y, int16_t z){
	offsets[0] = x;
	offsets[1] = y;
	offsets[2] = z;
}

void magnetometer_read_raw(int16_t* data){
	uint8_t message[6];
	message[0] = 0x06;
	twi_read_from(MAGNETOMETER_ADDRESS, message, 6, TWI_STOP);
	
	int16_t x = (((uint16_t) message[0]) << 8 | message[1]) + offsets[0];
	int16_t y = (((uint16_t) message[4]) << 8 | message[5]) + offsets[1];
	int16_t z = (((uint16_t) message[2]) << 8 | message[3]) + offsets[2];
	
	if (x >= -500 && x <= 500 && y >= -500 && y <= 500 && z >= -500 && z <= 500){
		raw[0] = ALPHA * x + (1 - ALPHA) * raw[0];
		raw[1] = ALPHA * y + (1 - ALPHA) * raw[1];
		raw[2] = ALPHA * z + (1 - ALPHA) * raw[2];
	}
	
	data[0] = raw[0];
	data[1] = raw[1];
	data[2] = raw[2];
	
	message[0] = 0x03;
	twi_write_to(MAGNETOMETER_ADDRESS, message, 1, TWI_BLOCK, TWI_STOP);
}

double magnetometer_read_heading(){
	int16_t data[3];
	magnetometer_read_raw(data);
	return atan2(data[1], data[0]);
}