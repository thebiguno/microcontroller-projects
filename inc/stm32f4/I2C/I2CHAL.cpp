//Implementation of AVR I2C object.  This is a thin wrapper around the lower level twi library.
#include "I2CHAL.h"
#include "stm32f4xx_hal_fmpi2c.h"

using namespace digitalcave;

I2CHAL::I2CHAL(I2C_HandleTypeDef* hi2c) :
	hi2c(hi2c)
{
	HAL_I2C_Init(hi2c);
}

void I2CHAL::read(uint8_t address, I2CMessage* m, uint8_t length, uint8_t i2cStop){
	HAL_I2C_Master_Receive(hi2c, address << 1, m->getData(), m->getLength(), 100);
}

void I2CHAL::write(uint8_t address, I2CMessage* m, uint8_t i2cBlock, uint8_t i2cStop){
	HAL_I2C_Master_Transmit(hi2c, address << 1, m->getData(), m->getLength(), 100);
}
