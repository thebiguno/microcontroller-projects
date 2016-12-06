#include "HMC5883L.h"

using namespace digitalcave;

HMC5883L::HMC5883L(I2C* i2c, vector_t calibration) :
	i2c(i2c),
	calibration(calibration)
{
	uint8_t data[6];
	I2CMessage message(data, sizeof(data));

	message.setLength(2);
	delay_ms(10);
	data[0] = HMC5883L_CONFIG_REG_A;
	data[1] = 0x78;		//8 sample average, 75 Hz, normal mode
	i2c->write(HMC5883L_ADDRESS, &message);

	data[0] = HMC5883L_CONFIG_REG_B;
	data[1] = 0x20;		//Gain 1 (± 1.3 Ga)
	i2c->write(HMC5883L_ADDRESS, &message);

	data[0] = HMC5883L_CONFIG_MODE;
	data[1] = 0x00;		//Continuous measurement mode
	i2c->write(HMC5883L_ADDRESS, &message);

	//Discard the first reading after mode change
	message.setLength(1);
	data[0] = HMC5883L_CONFIG_DATA_OUTPUT_X_MSB;
	i2c->write(HMC5883L_ADDRESS, &message);
	message.setLength(6);
	i2c->read(HMC5883L_ADDRESS, &message);
}

void HMC5883L::getRaw(uint8_t* raw){
	uint8_t data[6];
	I2CMessage message(data, sizeof(data));

	message.setLength(1);
	data[0] = HMC5883L_CONFIG_DATA_OUTPUT_X_MSB;
	i2c->write(HMC5883L_ADDRESS, &message);

	message.setLength(6);
	i2c->read(HMC5883L_ADDRESS, &message);

	//Magnetometer returns values in X, Z, Y ordering; we change it to X, Y, Z
	raw[0] = data[0];
	raw[1] = data[1];
	raw[2] = data[4];
	raw[3] = data[5];
	raw[4] = data[2];
	raw[5] = data[3];

}

inline vector_t getValuesConverted(uint8_t* raw, vector_t calibration){
	int16_t x = ((int16_t) raw[0]) << 8 | raw[1];
	int16_t y = ((int16_t) raw[2]) << 8 | raw[3];
	int16_t z = ((int16_t) raw[4]) << 8 | raw[5];

	vector_t result;
	result.x = x + calibration.x;
	result.y = y + calibration.y;
	result.z = z + calibration.z;

	return result;
}

vector_t HMC5883L::getValues(){
	uint8_t raw[6];
	getRaw(raw);

	return getValuesConverted(raw, calibration);
}

vector_t HMC5883L::getValuesFromRaw(uint8_t* raw){
	return getValuesConverted(raw, calibration);
}

double HMC5883L::getHeading(vector_t values){
	double radians = atan2(values.y, values.x);

	while (radians > M_PI){
		radians += (M_PI * -2);
	}
	while (radians < (M_PI * -1)){
		radians += (M_PI * 2);
	}
	return radians;
}

void HMC5883L::calibrate(){
	//We do a simplistic calibration here by averaging the min and max of each axis.
	// In theory this should be the center, assuming you get all the way around.
	vector_t min = {0, 0, 0};
	vector_t max = {0, 0, 0};
	vector_t zero = {0, 0, 0};

	for (uint16_t i = 0; i < 20000; i++){
		uint8_t raw[6];
		getRaw(raw);
		vector_t reading = getValuesConverted(raw, zero);

		if (reading.x < min.x && reading.x > -500) min.x = reading.x;
		if (reading.y < min.y && reading.y > -500) min.y = reading.y;
		if (reading.z < min.z && reading.z > -500) min.z = reading.z;
		if (reading.x > max.x && reading.x < 500) max.x = reading.x;
		if (reading.y > max.y && reading.y < 500) max.y = reading.y;
		if (reading.z > max.z && reading.z < 500) max.z = reading.z;

		delay_ms(1);
	}

	calibration.x = (max.x + min.x) / 2;
	calibration.y = (max.y + min.y) / 2;
	calibration.z = (max.z + min.z) / 2;
}
