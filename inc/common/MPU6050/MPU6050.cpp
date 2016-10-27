#include "MPU6050.h"

using namespace digitalcave;

//Inits the MPU6050 control object and sends the power up commands to the chip.
MPU6050::MPU6050(I2C* i2c) :
    i2c(i2c),
    calibration()
{
	uint8_t data[2];
	I2CMessage message(data, sizeof(data));

	//Reset MPU
	data[0] = MPU6050_PWR_MGMT_1;
	data[1] = 0x80;
	i2c->write(MPU6050_ADDRESS, &message);

	delay_ms(100);

	//Wake up
	data[0] = MPU6050_PWR_MGMT_1;
	data[1] = 0x00;
	i2c->write(MPU6050_ADDRESS, &message);	//Disable sleep (required at startup)

	delay_ms(500);

	//Set gyro to 2000 deg/s range
	data[0] = MPU6050_GYRO_CONFIG;
	data[1] = 0x18;		//2000 deg/s
	i2c->write(MPU6050_ADDRESS, &message);

	//Set accel to +/- 16g
	data[0] = MPU6050_ACCEL_CONFIG;
	data[1] = 0x18;		//+/- 16g
	i2c->write(MPU6050_ADDRESS, &message);

	//Digital LPF config.
	data[0] = MPU6050_CONFIG;
	data[1] = 0x02;		//Accel = 94Hz, Gyro = 98 Hz
	i2c->write(MPU6050_ADDRESS, &message);

	//Output rate config
	data[0] = MPU6050_SMPLRT_DIV;
	data[1] = 0x00;		//1kHz sample rate
	i2c->write(MPU6050_ADDRESS, &message);
}

//You should only call this when the chip is on a flat, nonmoving surface.  We find the average error
// and set the calibration variables accordingly.  You can read the calibration via get*Calib(), and persist
// to EEPROM if desired.
void MPU6050::calibrate(){
	int32_t totals[6] = {0, 0, 0, 0, 0, 0};
	const uint16_t count = 1000;
	uint8_t data[14];
	I2CMessage message(data, 1);
	for (uint16_t i = 0; i < count; i++){
		message.setLength(1);
		data[0] = MPU6050_ACCEL_XOUT_H;
		i2c->write(MPU6050_ADDRESS, &message);				//Go to register MPU6050_ACCEL_XOUT_H
		message.setLength(14);
		i2c->read(MPU6050_ADDRESS, &message);				//Read 14 bytes (message length): Accel X/Y/Z, temp, Gyro X/Y/Z, 16 bits signed each

		totals[0] += (int16_t) (((uint16_t) data[0] << 8) | data[1]);
		totals[1] += (int16_t) (((uint16_t) data[2] << 8) | data[3]);
		totals[2] += (int16_t) (((uint16_t) data[4] << 8) | data[5]);
		totals[3] += (int16_t) (((uint16_t) data[8] << 8) | data[9]);
		totals[4] += (int16_t) (((uint16_t) data[10] << 8) | data[11]);
		totals[5] += (int16_t) (((uint16_t) data[12] << 8) | data[13]);

		delay_ms(1);
	}

	//We want Accel Z (index 2) to be 1g, and all else to be 0.
	calibration[0] = 0 - totals[0] / count;
	calibration[1] = 0 - totals[1] / count;
	calibration[2] = 2048 - totals[2] / count;		//2048 = 32768 (full scale range) divided by 16 (16g), i.e. what a nominal 1G reading should be
	calibration[3] = 0 - totals[3] / count;
	calibration[4] = 0 - totals[4] / count;
	calibration[5] = 0 - totals[5] / count;
}

inline vector_t getAccelConverted(uint8_t* data, MPU6050* mpu6050){
	vector_t result;
	int16_t* calibration = mpu6050->getCalibration();
	result.x = (((int16_t) (((uint16_t) data[0] << 8) | data[1])) + calibration[0]) * 0.00048828125;		// 16g / 32768
	result.y = (((int16_t) (((uint16_t) data[2] << 8) | data[3])) + calibration[1]) * 0.00048828125;
	result.z = (((int16_t) (((uint16_t) data[4] << 8) | data[5])) + calibration[2]) * 0.00048828125;
	return result;
}

inline vector_t getGyroConverted(uint8_t* data, MPU6050* mpu6050){
	vector_t result;
	int16_t* calibration = mpu6050->getCalibration();
	result.x = (((int16_t) (((uint16_t) data[0] << 8) | data[1])) + calibration[3]) * (0.06103515625 * M_PI / 180);		//(2000 deg/s / 32768) * PI/180.
	result.y = (((int16_t) (((uint16_t) data[2] << 8) | data[3])) + calibration[4]) * (0.06103515625 * M_PI / 180);
	result.z = (((int16_t) (((uint16_t) data[4] << 8) | data[5])) + calibration[5]) * (0.06103515625 * M_PI / 180);
	return result;
}

inline double getTemperatureConverted(uint8_t* data){
	return (((int16_t) (((uint16_t) data[0] << 8) | data[1])) / 340.0) + 36.53;	//equation for temperature in degrees C from datasheet
}

//Returns a vector of acceleration values (in g)
vector_t MPU6050::getAccel(){
	uint8_t data[6];
	I2CMessage message(data, 1);
	data[0] = MPU6050_ACCEL_XOUT_H;
	i2c->write(MPU6050_ADDRESS, &message);				//Go to register MPU6050_ACCEL_XOUT_H
	message.setLength(6);
	i2c->read(MPU6050_ADDRESS, &message);				//Read 6 bytes (Accel X/Y/Z, 16 bits signed each)

 	return getAccelConverted(data, this);
}

//Returns a vector of gyroscope values (in rad / s)
vector_t MPU6050::getGyro(){
	uint8_t data[6];
	I2CMessage message(data, 1);
	data[0] = MPU6050_GYRO_XOUT_H;
	i2c->write(MPU6050_ADDRESS, &message);				//Go to register MPU6050_GYRO_XOUT_H
	message.setLength(6);
	i2c->read(MPU6050_ADDRESS, &message);				//Read 14 bytes (Gyro X/Y/Z, 16 bits signed each)

	return getGyroConverted(data, this);
}

//Returns the temperature (in C)
double MPU6050::getTemperature(){
	uint8_t data[2];
	I2CMessage message(data, 1);
	data[0] = MPU6050_TEMP_OUT_H;
	i2c->write(MPU6050_ADDRESS, &message);				//Go to register MPU6050_TEMP_OUT_H
	message.setLength(2);
	i2c->read(MPU6050_ADDRESS, &message);				//Read 2 bytes (16 bit raw temperature data)

	return getTemperatureConverted(data);
}

void MPU6050::getValues(vector_t* accel, vector_t* gyro, double* temperature){
	uint8_t data[14];
	I2CMessage message(data, 1);
	data[0] = MPU6050_ACCEL_XOUT_H;
	i2c->write(MPU6050_ADDRESS, &message);				//Go to register MPU6050_ACCEL_XOUT_H
	message.setLength(14);
	i2c->read(MPU6050_ADDRESS, &message);				//Read 6 bytes (Accel X/Y/Z, temp, Gyro X/Y/Z, 16 bits signed each)

	getValuesFromRaw(accel, gyro, temperature, data);
}

void MPU6050::getValuesFromRaw(vector_t* accel, vector_t* gyro, double* temperature, uint8_t* data){
	*accel = getAccelConverted(data + 0, this);
	*gyro = getGyroConverted(data + 8, this);
	*temperature = getTemperatureConverted(data + 6);
}

void MPU6050::getRaw(uint8_t* raw){
	uint8_t data[14];
	I2CMessage message(data, 1);
	data[0] = MPU6050_ACCEL_XOUT_H;
	i2c->write(MPU6050_ADDRESS, &message);				//Go to register MPU6050_ACCEL_XOUT_H
	message.setLength(14);
	i2c->read(MPU6050_ADDRESS, &message);				//Read 6 bytes (Accel X/Y/Z, temp, Gyro X/Y/Z, 16 bits signed each)

	for (uint8_t i = 0; i < 14; i++){
		raw[i] = data[i];
	}
}

//Get / set calibration data.  Order is Accel X, Y, Z, Gyro X, Y, Z, sent as an int16_t array.
// These functions can be used to persist to / from EEPROM from main program.
void MPU6050::setCalibration(int16_t* calibration){
	for(uint8_t i = 0; i < 6; i++){
		this->calibration[i] = calibration[i];
	}
}
