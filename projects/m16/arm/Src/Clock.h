#ifndef Clock_H
#define Clock_H

#include "Module.h"
#include <stdint.h>
#include "stm32f4xx_hal_rtc.h"

namespace digitalcave {
    class Clock : public Module {
    private:
        RTC_DateTypeDef date;
        RTC_TimeTypeDef time;
        uint8_t stale;
        uint8_t frame;
    public:
        Clock();
        ~Clock();
        void paint();
    };
}

#endif
