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

void HMC5883L::calibrate(){
	//We do a simplistic calibration here by averaging each axis value.  Assuming you are turning it
	// at a constant rate, over time the average on each axis should be the center.
	
}
