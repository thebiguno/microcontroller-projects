#include "stm32f4xx_hal.h"

#include <SerialHAL.h>
#include <ESP8266.h>
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
	wifi.join((char*)"sloth", (char*)"password");
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
	wifi.start_server(1234);
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
	wifi.start_mdns((char*)"echoserver", (char*)"telnet", 1234);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);

	HAL_Delay(1000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_Delay(100);

	while (1) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);

		uint8_t b = 0;
		ESP8266Socket* socket = wifi.accept();
		if (socket != NULL) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_Delay(100);

			socket->write("Hello Socket\n");
			socket->flush();
			while (!socket->is_closed()) {
				if (socket->read(&b)) {
					socket->write(b);
					if (b == '\n') {
						socket->flush();
					}
					HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
				}
			}
		}
	}
}
