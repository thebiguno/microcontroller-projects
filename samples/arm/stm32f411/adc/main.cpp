#include "stm32f4xx_hal.h"

#include <SerialHAL.h>
#include <TimerHAL.h>

using namespace digitalcave;

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;

extern "C" {
	void dc_main();
}

void dc_main(){
	//We flash the light briefly at startup
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	
	SerialHAL serial(&huart2, 64);
	
	uint32_t lastSend = timer_millis();
	while (1){
		if ((timer_millis() - lastSend) > 500){
			HAL_ADC_Start(&hadc1);
			HAL_ADC_PollForConversion(&hadc1, 1000);
			uint16_t value = HAL_ADC_GetValue(&hadc1);
			HAL_ADC_Stop(&hadc1);
			
			char buffer[32];
			snprintf(buffer, sizeof(buffer), "ADC1_0: %d\n", value);
			serial.write(buffer);
			lastSend = timer_millis();
		}
	}
}
