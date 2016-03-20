#include "stm32f4xx_hal.h"

#include <SerialHAL.h>
#include <TimerHAL.h>

using namespace digitalcave;

extern UART_HandleTypeDef huart2;

extern "C" {
	void dc_main();
}

SerialHAL* serial;

void dc_main(){
	//We flash the light briefly, then go into the infinite loop.  The WDT should kick in at some point...
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	
	SerialHAL s(&huart2, 64);
	serial = &s;
	
	uint32_t lastSend = timer_millis();
	while (1){
		if ((timer_millis() - lastSend) > 500){
			char buffer[32];
			snprintf(buffer, sizeof(buffer), "Time: %d\n", (uint16_t) timer_millis());
			serial->write(buffer);
			lastSend = timer_millis();
		}
		
		uint8_t b = 0;
		if (serial->read(&b)){
			serial->write(b);
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if (serial){
		serial->isr();
	}
}