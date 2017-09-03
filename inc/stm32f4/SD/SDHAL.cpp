/*
 * Much of this code was inspired by https://luckyresistor.me/cat-protector/software/sdcard-2/
 * and http://alumni.cs.ucr.edu/~amitra/sdcard/Additional/sdcard_appnote_foust.pdf
 * and https://github.com/LonelyWolf/stm32/blob/master/cube-usb-msc/sdcard-sdio.c
 */
#include "SD.h"
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_sd.h>

using namespace digitalcave;

SD::SD(uint8_t wide) :
	block(0),
	position(0)
{
	// 0. Initialize the GPIO
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Mode = GPIO_Mode_AF; // Alternative function mode
	gpio.GPIO_Speed = GPIO_Speed_40MHz; // High speed
	gpio.GPIO_OType = GPIO_OType_PP; // Output push-pull
	gpio.GPIO_PuPd = GPIO_PuPd_UP; // Pull-up

	// SDIO_CMD (PD2)
	gpio.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &gpio);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,SDIO_GPIO_AF);

	// SDIO_D0 (PC8)
	gpio.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOC, &gpio);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,SDIO_GPIO_AF);

	// SDIO_CK (PC12)
	gpio.GPIO_Pin = GPIO_Pin_12;
	GPIO_Init(GPIOC, &gpio);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,SDIO_GPIO_AF);

	if (wide) {
		// SDIO_D1 (PC9)
		gpio.GPIO_Pin = GPIO_Pin_9;
		GPIO_Init(GPIOC, &gpio);
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,SDIO_GPIO_AF);

		// SDIO_D2 (PC10)
		gpio.GPIO_Pin = GPIO_Pin_10;
		GPIO_Init(GPIOC, &gpio);
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,SDIO_GPIO_AF);

		// SDIO_D3 (PC11)
		gpio.GPIO_Pin = GPIO_Pin_11;
		GPIO_Init(GPIOC, &gpio);
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,SDIO_GPIO_AF);
	}

	// 1. Initialize the SDIO peripheral interface with defaullt configuration.
	hsd.Instance = SDIO;
	hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
	hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
	hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
	hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
	hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
	hsd.Init.ClockDiv = 120; // 48MHz / (118+2) = 400KHz
	HAL_SD_Init(&hsd);
	SDIO_PowerState_ON();

	// 2. Initialize the SD card.
	HAL_SD_InitCard(&hsd);

	// 3. Configure the SD Card Data transfer frequency.
	hsd.Init.ClockDiv = 0; // 48MHz / (0+2) = 24MHz
	HAL_SD_Init(&hsd);

	if (wide) {
		HAL_SD_WideBusOperation_Config(&hsd, SDIO_BUS_WIDE_4B);
	}
}

SD::~SD() {
	HAL_SD_DeInit(hsd);
	SDIO_PowerState_OFF();
}

void SD::setBlock(uint32_t block) {
	this->block = block;
	this->position = 0;
}

uint16_t SD::skip(uint16_t len) {
	if (len > 512 || 512 - len < this->position) {
		len = 512 - this->position;
	}
	this->position += len;
	return len;
}

uint8_t SD::read(uint8_t* a){
	uint8_t result;
	uint16_t read = this->read(&result, 1);
	return (read == 1) ? result : 0;
}

uint16_t read(uint8_t* a, uint16_t len) {
	HAL_SD_ReadBlocks(&hsd, a, block, 1, uint32_t Timeout);
	HAL_SD_GetCardState(&hsd);
}

// TODO for now just read-only
uint8_t SD::write(uint8_t b) {
	return 0;
}
