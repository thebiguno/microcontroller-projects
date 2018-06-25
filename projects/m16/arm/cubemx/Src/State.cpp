#include "State.h"
#include <stdlib.h>
#include <util/delay.h>
#include <math.h>

using namespace digitalcave;

State::State() {
}

State::~State() {
}

void readEeprom() {
    I2C_HandleTypeDef hi2c;
    hi2c.Instance = I2C1;
    hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c.Init.ClockSpeed = 400000;
    hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
    hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    hi2c.Init.OwnAddress1 = 0;
    hi2c.Init.OwnAddress2 = 0;
    I2CHAL(&hi2c);

}