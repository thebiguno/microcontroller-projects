#include "stm32f4xx_hal.h"

#include <persist.h>
#include <dctypes.h>
#include <dcutil/delay.h>
#include <SerialHAL.h>

using namespace digitalcave;

extern UART_HandleTypeDef huart6;

extern "C" {
	void dc_main();
}


void dc_main(){
	//We flash the green light briefly.
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
	for (uint8_t i = 0; i < 10; i++){
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
		delay_ms(200);
	}

	SerialHAL serial(&huart6, 64);

	//We check whether the persisted flash is valid
	uint8_t data[8];
	persist_read(0, data, 8);

	char buffer[32];
	snprintf(buffer, sizeof(buffer), "%02x %02x %02x %02x %02x %02x %02x %02x \n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
	serial.write(buffer);

	if (data[0] == 0x42 && data[1] == 0x00){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);

		serial.write("Flash verified\n");
	}
	else {
		//If not, we show a red light and write it now
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);
		data[0] = 0x42;
		data[1] = 0x00;
		data[2] = 0x01;
		data[3] = 0x02;
		data[4] = 0x03;
		data[5] = 0x04;
		data[6] = 0x05;
		data[7] = 0x06;
		persist_write(0, data, 8);
		serial.write("Flash updated\n");

		persist_read(0, data, 8);

		char buffer[32];
		snprintf(buffer, sizeof(buffer), "%02x %02x %02x %02x %02x %02x %02x %02x \n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
		serial.write(buffer);
	}

	while (1){
	}
}
