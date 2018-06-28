#ifndef Clock_H
#define Clock_H

#include "Module.h"
#include <stdint.h>

namespace digitalcave {
    class Clock : public Module {
    private:
        RTC_DateTypeDef date;
        RTC_TimeTypeDef time;
        uint8_t stale;
    public:
        Clock();
        ~Clock();
        void paint();
    };
}

#endif