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

	ESP8266 wifi(serial);
	wifi.join("sloth","password");
	wifi.start_server(1234);
	wifi.start_mdns("echoserver", "telnet", 1234);

	while (1) {
		uint8_t b = 0;
		ESP8266Socket* socket = wifi.accept();
		if (socket != NULL && !socket->is_closed()) {
			socket->read(&b);
			socket->write(b);
		}
	}

}
