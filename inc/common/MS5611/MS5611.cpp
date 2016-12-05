#include "MS5611.h"

using namespace digitalcave;

MS5611::MS5611(I2C* i2c, uint8_t oversampling) :
	i2c(i2c),
	oversampling(oversampling)
{
	if (oversampling == MS5611_OVERSAMPLE_ULTRA_LOW_POWER){
		conversionDelay = 1;
	}
	else if (oversampling == MS5611_OVERSAMPLE_LOW_POWER){
		conversionDelay = 2;
	}
	else if (oversampling == MS5611_OVERSAMPLE_STANDARD){
		conversionDelay = 3;
	}
	else if (oversampling == MS5611_OVERSAMPLE_HIGH_RES){
		conversionDelay = 5;
	}
	else {
		conversionDelay = 10;
	}


	uint8_t data[2];
	I2CMessage message(data, sizeof(data));

	//Send reset signal
	delay_ms(100);
	message.setLength(1);
	data[0] = MS5611_CMD_RESET;
	i2c->write(MS5611_ADDRESS, &message);				//Send reset command
	delay_ms(100);


	//Read the factory calibration data in PROM
	for (uint8_t i = 0; i < 8; i++){
		if (i == 0){
			coefficient[i] = 0;									//Coefficient 0 is reserved and meaningless.
		}
		else {
			message.setLength(1);
			data[0] = MS5611_CMD_READ_PROM + i * 2;				//The 4 MSB == 0xA for PROM read mode; the 4 LSB == 0..7.  Index 0 is reserved by manufacturer; coefficent 0 is at index 1, etc.  Index 7 is the CRC.
			i2c->write(MS5611_ADDRESS, &message);				//Go to register PROM read plus index.
			message.setLength(2);
			i2c->read(MS5611_ADDRESS, &message);				//Read 2 bytes, 16 bits unsigned

			coefficient[i] = data[0] << 8 | data[1];
		}
	}
}

void MS5611::getRaw(uint8_t* raw){
	uint8_t data[3];
	I2CMessage message(data, sizeof(data));

	message.setLength(1);
	data[0] = MS5611_CMD_CONV_D1 + oversampling;
	i2c->write(MS5611_ADDRESS, &message);				//Start D1 conversion with desired oversampling

	delay_ms(conversionDelay);

	data[0] = MS5611_CMD_ADC_READ;
	i2c->write(MS5611_ADDRESS, &message);				//Read result of D1 conversion
	message.setLength(3);
	i2c->read(MS5611_ADDRESS, &message);				//Read 3 bytes, 24 bits unsigned

	raw[0] = data[0];
	raw[1] = data[1];
	raw[2] = data[2];

	message.setLength(1);
	data[0] = MS5611_CMD_CONV_D2 + oversampling;
	i2c->write(MS5611_ADDRESS, &message);				//Start D2 conversion with desired oversampling

	delay_ms(conversionDelay);

	data[0] = MS5611_CMD_ADC_READ;
	i2c->write(MS5611_ADDRESS, &message);				//Read result of D1 conversion
	message.setLength(3);
	i2c->read(MS5611_ADDRESS, &message);				//Read 3 bytes, 24 bits unsigned

	raw[3] = data[0];
	raw[4] = data[1];
	raw[5] = data[2];
}

inline int32_t getDt(uint8_t* raw, uint16_t* coefficient){
	uint32_t d2 = raw[3] << 16 | raw[4] << 8 | raw[5];

	int32_t dt = d2 - (coefficient[5] << 8);
	return dt;
}

inline int32_t getTemperatureConverted(int32_t dt, uint16_t* coefficient){
	int32_t temp = 2000 + (((int64_t) dt * coefficient[6]) >> 23);

	if (temp < 2000){
		temp = temp - (((int64_t) dt * dt) >> 31);
	}
	return temp;
}

inline int32_t getPressureConverted(uint8_t* raw, uint16_t* coefficient){
	//See datasheet page 8 for variable naming and formulae
	uint32_t d1 = raw[0] << 16 | raw[1] << 8 | raw[2];

	int32_t dt = getDt(raw, coefficient);
	int32_t temp = getTemperatureConverted(dt, coefficient);

	int64_t off = ((int64_t) coefficient[2] << 16) + (((int64_t) coefficient[4] * dt) >> 7);
	int64_t sens = ((int64_t) coefficient[1] << 15) + (((int64_t) coefficient[3] * dt) >> 8);

	int64_t off2 = 0;
	int64_t sens2 = 0;

	if (temp < 2000){
		off2 = (5 * ((temp - 2000) * (temp - 2000)) >> 1);
		sens2 = off2;
	}

	if (temp < -1500){
		off2 = off2 + 7 * (temp + 1500) * (temp + 1500);
		sens2 = sens2 + ((11 * (temp + 1500) * (temp + 1500)) >> 1);
	}

	off = off - off2;
	sens = sens - sens2;

	int64_t p = ((((int64_t) d1 * sens) >> 21) - off) >> 15;

	return p;
}

int32_t MS5611::getPressure(){
	uint8_t raw[6];
	getRaw(raw);

	return getPressureConverted(raw, coefficient);
}

int32_t MS5611::getPressureFromRaw(uint8_t* raw){
	return getPressureConverted(raw, coefficient);
}

int32_t MS5611::getTemperature(){
	//See datasheet page 8 for variable naming and formulae
	uint8_t raw[6];
	getRaw(raw);

	return getTemperatureConverted(getDt(raw, coefficient), coefficient);
}

int32_t MS5611::getTemperatureFromRaw(uint8_t* raw){
	return getTemperatureConverted(getDt(raw, coefficient), coefficient);
}
