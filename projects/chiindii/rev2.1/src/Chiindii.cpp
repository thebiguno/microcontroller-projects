#include "stm32f4xx_hal.h"

#include "Chiindii.h"

using namespace digitalcave;

extern "C" {
	void chiindii_main();
}


void chiindii_main(){
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
	while (1){
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
		HAL_Delay(1000);
	}
}
