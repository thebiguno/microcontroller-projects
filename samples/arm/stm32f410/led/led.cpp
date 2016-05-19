#include "stm32f4xx_hal.h"

#include <SerialHAL.h>
#include <TimerHAL.h>

using namespace digitalcave;

extern "C" {
	void led_main();
}


void led_main(){
	HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
	while (1){
		HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
		HAL_Delay(1000);	
	}
}