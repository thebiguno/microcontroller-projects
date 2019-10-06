#include "stm32f4xx_hal.h"

#include "Inc/main.h"

//using namespace digitalcave;

extern "C" {
	void loop();
}

void loop() {

    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    HAL_Delay(1000);

}
