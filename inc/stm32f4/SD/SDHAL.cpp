/*
 * Much of this code was inspired by https://luckyresistor.me/cat-protector/software/sdcard-2/
 * and http://alumni.cs.ucr.edu/~amitra/sdcard/Additional/sdcard_appnote_foust.pdf
 * and https://github.com/LonelyWolf/stm32/blob/master/cube-usb-msc/sdcard-sdio.c
 */
#include "SDHAL.h"

using namespace digitalcave;

SD::SD(uint8_t wide) :
	block(0),
	position(0)
{
	// 0. Initialize the GPIO
	GPIO_InitTypeDef gpio;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio.Pull = GPIO_PULLUP;

	// SDIO_CMD (PD2)
	gpio.Pin = GPIO_PIN_2;
	gpio.Alternate = GPIO_AF12_SDIO;
	HAL_GPIO_Init(GPIOD, &gpio);

	// SDIO_D0 (PC8)
	gpio.Pin = GPIO_PIN_8;
	gpio.Alternate = GPIO_AF12_SDIO;
	HAL_GPIO_Init(GPIOC, &gpio);

	// SDIO_CK (PC12)
	gpio.Pin = GPIO_PIN_12;
	gpio.Alternate = GPIO_AF12_SDIO;
	HAL_GPIO_Init(GPIOC, &gpio);

	if (wide) {
		// SDIO_D1 (PC9)
		gpio.Pin = GPIO_PIN_9;
		gpio.Alternate = GPIO_AF12_SDIO;
		HAL_GPIO_Init(GPIOC, &gpio);

		// SDIO_D2 (PC10)
		gpio.Pin = GPIO_PIN_10;
		gpio.Alternate = GPIO_AF12_SDIO;
		HAL_GPIO_Init(GPIOC, &gpio);

		// SDIO_D3 (PC11)
		gpio.Pin = GPIO_PIN_11;
		gpio.Alternate = GPIO_AF12_SDIO;
		HAL_GPIO_Init(GPIOC, &gpio);
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

	// 2. Initialize the SD card.
	HAL_SD_InitCard(&hsd);

	// 3. Configure the SD Card Data transfer frequency.
	hsd.Init.ClockDiv = 0; // 48MHz / (0+2) = 24MHz
	HAL_SD_Init(&hsd);

	if (wide) {
		HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B);
	}
}

SD::~SD() {
	HAL_SD_DeInit(&hsd);
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

uint16_t SD::read(uint8_t* a, uint16_t len) {
	HAL_SD_ReadBlocks(&hsd, a, block, 1, 1);
	HAL_SD_GetCardState(&hsd);
	return 1;
}

// TODO for now just read-only
uint8_t SD::write(uint8_t b) {
	return 0;
}
