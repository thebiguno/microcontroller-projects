//Implementation of AVR I2C object.  This is a thin wrapper around the lower level twi library.
#include "I2CHAL.h"

using namespace digitalcave;

I2CHAL::I2CHAL(I2C_HandleTypeDef* hi2c) :
	hi2c(hi2c)
{
	HAL_I2C_Init(hi2c);
}
