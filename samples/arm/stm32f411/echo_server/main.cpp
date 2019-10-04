#include "stm32f4xx_hal.h"

#include <SerialHAL.h>
#include <ESP8266.h>
#include <ESP8266Stream.h>
#include <TimerHAL.h>

using namespace digitalcave;

extern UART_HandleTypeDef huart1;

extern "C" {
	void loop();
}

SerialHAL* serial;

void loop() {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);

	SerialHAL s(&huart1, 64);
	serial = &s;
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);

	ESP8266 wifi(serial);
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
	wifi.at_cwjap_cur((char*)"sloth", (char*)"password");
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
	wifi.at_cipserver_tcp(1, 1234);
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
	wifi.at_mdns(1, (char*)"echoserver", (char*)"telnet", 1234);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);

	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_Delay(100);

	while (1) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);

		uint8_t b = 0;
		ESP8266Stream* stream = wifi.accept();
		if (stream != NULL) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_Delay(100);

			stream->write("Hello Socket\n");
			stream->flush();
			while (!stream->is_closed()) {
				if (stream->read(&b)) {
					stream->write(b);
					if (b == '\n') {
						stream->flush();
					}
					HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
				}
			}
		}
	}
}
