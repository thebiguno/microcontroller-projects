#include "stm32f4xx_hal.h"

#include "main.h"
#include "Src/ph42121rgb16s.h"
#include "Src/App.h"
#include "Src/Matrix.h"

extern "C" {
	void run();
}

using namespace digitalcave;

void run() {
    ph42121rgb16s_init();

    App app = App();

    while (1) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        HAL_Delay(1000);

        app.run();
    }
}

extern "C" void __cxa_pure_virtual() { while (1); }
