#include "stm32f4xx_hal.h"

#include <SerialHAL.h>
#include <ESP8266.h>
#include <TimerHAL.h>

using namespace digitalcave;

extern UART_HandleTypeDef huart1;

extern "C" {
	void dc_main();
}

SerialHAL* serial;

void dc_main(){
	SerialHAL s(&huart1, 64);
	serial = &s;

	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	HAL_Delay(100);
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	HAL_Delay(100);

	ESP8266 wifi(serial);
	wifi.join((char*)"sloth", (char*)"password");
	wifi.start_server(1234);
	wifi.start_mdns((char*)"echoserver", (char*)"telnet", 1234);

	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	HAL_Delay(100);
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	HAL_Delay(100);

	while (1) {
		uint8_t b = 0;
		ESP8266Socket* socket = wifi.accept();
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_7);
		HAL_Delay(100);
		if (socket != NULL) {
			socket->write("Hello Socket\n");
			socket->flush();
			while (!socket->is_closed()) {
				if (socket->read(&b)) {
					socket->write(b);
					if (b == '\n') {
						socket->flush();
					}
					HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
				}
			}
		}
	}
}
