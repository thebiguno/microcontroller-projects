#include "State.h"
#include <stdlib.h>
#include <math.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_rtc.h"

using namespace digitalcave;

State::State() {
}

State::~State() {
}

void State::readEeprom() {
    uint16_t DevAddress;
    uint16_t MemAddress;
    uint16_t MemAddSize;
    uint8_t *pData;
    uint16_t Size;
    uint32_t Timeout;

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

    HAL_StatusTypeDef result = HAL_I2C_Mem_Write(&hi2c, 0xA0 & 0x00,
            address + Counter, I2C_MEMADD_SIZE_8BIT,
            &MemTarget[Counter], EEPROM_MAXPKT, EEPROM_TIMEOUT);
    Counter += EEPROM_MAXPKT;

    while (Counter < Size && Result == HAL_OK) {
        uint16_t Diff = Size - Counter;

        if (Diff >= EEPROM_MAXPKT) {
            //Multi-Byte
            Result = HAL_I2C_Mem_Write(i2c_port, EEPROM_ADDRESS,
                    address + Counter, I2C_MEMADD_SIZE_16BIT,
                    &MemTarget[Counter], EEPROM_MAXPKT, EEPROM_TIMEOUT);
            Counter += EEPROM_MAXPKT;
        } else {
            //and the remaining ones...low packet size
            Result = HAL_I2C_Mem_Write(i2c_port, EEPROM_ADDRESS,
                    address + Counter, I2C_MEMADD_SIZE_16BIT,
                    &MemTarget[Counter], Diff, EEPROM_TIMEOUT);
            Counter += Diff;
        }
        HAL_Delay(EEPROM_WRITE);
    }
    return Result;
}

void State::writeEeprom() {
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

void State::readRtc(RTC_DateTypeDef *date, RTC_TimeTypeDef *time) {
    RTC_HandleTypeDef hrtc;
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BCD);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD);
}

void State::writeRtc(RTC_DateTypeDef *date, RTC_TimeTypeDef *time) {
    RTC_HandleTypeDef hrtc;

    sTime.TimeFormat = RTC_HourFormat_24;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_SET;
    HAL_RTC_SetTime(&hrtc, time, RTC_FORMAT_BCD);
    HAL_RTC_SetDate(&hrtc, date, RTC_FORMAT_BCD);
}
