#include "magnetometer.h"

#define MAGNETOMETER_ADDRESS 0x1E

void magnetometer_init(){
	twi_init();
	
	//Calibration
	uint8_t message[2];
	message[0] = 0x00;		//Register 0
	message[1] = 0x70;		//8 sample average, 15 Hz, normal mode
	twi_write_to(MAGNETOMETER_ADDRESS, message, 2, TWI_BLOCK, TWI_STOP);
	message[0] = 0x01;		//Register 1
	message[1] = 0xA0;		//Gain 5 (± 4.7 Ga)
	twi_write_to(MAGNETOMETER_ADDRESS, message, 2, TWI_BLOCK, TWI_STOP);
	message[0] = 0x02;		//Register 2
	message[1] = 0x00;		//Comtinuous measurement mode
	twi_write_to(MAGNETOMETER_ADDRESS, message, 2, TWI_BLOCK, TWI_STOP);
	
	int16_t data[3];
	magnetometer_read_raw(data);	//Disregard first reading after changing mode
	
//	while (data[0] < 200 | data[1] < 200 | data[2] < 200){
//		magnetometer_read_raw(data);
//	}

}

void magnetometer_read_raw(int16_t* data){
	uint8_t message[6];
	message[0] = 0x06;	//Read 4 bytes; we don't care about Z for now.
	twi_read_from(MAGNETOMETER_ADDRESS, message, 6, TWI_STOP);
	
	data[0] = ((uint16_t) message[0]) << 8 | message[1];
	data[1] = ((uint16_t) message[4]) << 8 | message[5];
	data[2] = ((uint16_t) message[2]) << 8 | message[3];
	
	message[0] = 0x03;
	twi_write_to(MAGNETOMETER_ADDRESS, message, 1, TWI_BLOCK, TWI_STOP);
}

void magnetometer_read_scaled(double* data){
	int16_t raw[3];
	magnetometer_read_raw(raw);
	
	data[0] = ((double) raw[0]) * 100 / 390;
	data[1] = ((double) raw[1]) * 100 / 390;
	data[2] = ((double) raw[2]) * 100 / 390;
}

double magnetometer_read_heading(){
	double data[3];
	magnetometer_read_scaled(data);
	return atan2(data[1], data[0]);
}