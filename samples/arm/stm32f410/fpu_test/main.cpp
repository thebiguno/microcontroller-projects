#include "stm32f4xx_hal.h"

#include <persist.h>
#include <dctypes.h>
#include <dcutil/delay.h>
#include <SerialHAL.h>
#include <TimerHAL.h>
#include <math.h>

using namespace digitalcave;

extern UART_HandleTypeDef huart6;

extern "C" {
	void dc_main();
}


void dc_main(){
	//This needs to be included to enable the FPU - see http://www.triplespark.net/elec/pdev/arm/stm32.html
	//Update: I don't think it is needed after all...
	//SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */

	//We flash the green light briefly.
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
	for (uint8_t i = 0; i < 10; i++){
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);
		delay_ms(200);
	}

	SerialHAL serial(&huart6, 64);

	while (1){
		//delay_ms(100);
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_0);

		uint32_t start = timer_millis();
		//Do a bunch of FPU operations to see timing details
		//Using double precision constants / sqrt, this takes about 105ms to complete.
		//Using single precision constants / sqrt and no FPU, this takes about 40ms to complete.
		//Using single precision constants / sqrt and hardware FPU, this takes about 34ms to complete.
		for (uint16_t i = 0; i < 10000; i++){
			float a = 3.1415926f;
			float b = 240.123f;
			a = a * a;
			a = a / b;
			a = b + a;
			a = b * a;
			a = b / a;
			a += 123.4567f;
			a = sqrtf(a * a + b * b);
		}
		uint32_t end = timer_millis();

		char buffer[32];
		snprintf(buffer, sizeof(buffer), "Time: %d ms\n", (uint16_t) (end - start));
		serial.write(buffer);
	}
}
