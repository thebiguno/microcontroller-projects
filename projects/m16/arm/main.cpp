#include "stm32f4xx_hal.h"

#include "main.h"
#include "Src/ph42121rgb16s.h"

//using namespace digitalcave;

extern "C" {
	void loop();
	void setup();
}

void setup() {
    ph42121rgb16s_init();
}

void loop() {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    HAL_Delay(1000);
}
