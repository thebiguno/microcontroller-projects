#include "stm32f4xx_hal.h"

#include <SerialHAL.h>
#include <TimerHAL.h>

using namespace digitalcave;

extern UART_HandleTypeDef huart2;
extern WWDG_HandleTypeDef hwwdg;

extern "C" {
	void wwdg_main();
}


void wwdg_main(){
	SerialHAL serial(&huart2, 64);
	
	//We flash the light briefly, then go into the infinite loop.  The WDT should kick in at some point...
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	
	//The WWDG runs on PCLK1 with a hardcoded prescaler of 4096.  The user prescaler is applied after, for a total 
	// frequency of FREQ = (PCLK1 / 4096) / prescaler.  You need to ensure that the refresh is called after the downcounter
	// reaches the window value, but before it reaches zero.  You can calculate these times using the formula
	// time (mS) = 1000 * (Window_Value) / FREQ.  So, assuming a 50MHz PCLK1, a div1 prescaler, and 64 for both window and counter,
	// this means that you must refresh before 1000 * (64) / (PCLK1 / 4096 / 1) = 5.2ms.
	// Assuming a 50MHz clock, div8 prescaler, 127 for downcounter and 64 for window, this means you need to wait for at least
	// 1000 * (127 - 64) / (PCLK1 / 4096 / 8) = 41.2ms and after the wait you have 1000 * (64) / (PCLK1 / 4096 / 8) = 41.9ms
	// to call refresh before it resets.
	HAL_WWDG_Start(&hwwdg);
	
	uint32_t lastRefresh = timer_millis();
	uint32_t lastSend = timer_millis();
	while (1){
		//A real program would call this regularly more frequently than the calculated period above.  For this sample, we want to
		// just force a reset to demonstrate how WDT works.
		if ((timer_millis() - lastRefresh) > 1){
			HAL_WWDG_Refresh(&hwwdg, 127);
			lastRefresh = timer_millis();
		}
		
		if ((timer_millis() - lastSend) > 500){
			char buffer[32];
			snprintf(buffer, sizeof(buffer), "Time: %d\n", (uint16_t) timer_millis());
			serial.write(buffer);
			lastSend = timer_millis();
		}
	}
}