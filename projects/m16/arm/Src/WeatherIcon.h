#ifndef WEATHER_ICON_H
#define WEATHER_ICON_H

#include "Module.h"
#include <stdint.h>

namespace digitalcave {
    class WeatherIcon : public Module {
    private:
        char* icon;

    public:
        WeatherIcon(char* i);
        ~WeatherIcon();
        void paint();
    };
}

#endif