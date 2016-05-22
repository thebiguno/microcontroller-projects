#include "stm32f4xx_hal.h"

#include <SerialHAL.h>
#include <TimerHAL.h>

using namespace digitalcave;

extern TIM_HandleTypeDef htim5;

extern "C" {
	void motor_main();
}


void motor_main(){
	HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);
	
	HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
	while (1){
		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
		
		for (uint16_t i = 0; i < htim5.Init.Period; i++){
			__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_1, i);
			HAL_Delay(2);
		}
		
		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
		
		for (uint16_t i = htim5.Init.Period; i > 0; i--){
			__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_1, i);
			HAL_Delay(2);
		}
	}
}
