#include <util/delay.h>
#include "stm32f4xx_hal.h"

void delay_ms(uint32_t delay){
	HAL_Delay(delay);
}
