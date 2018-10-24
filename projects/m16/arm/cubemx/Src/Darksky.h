#ifndef DARKSKY_H
#define DARKSKY_H

#include <stdint.h>

#define ICON_UNKNOWN 0
#define ICON_CLEAR_DAY 1
#define ICON_CLOUDY_DAY 2
#define ICON_CLOUDY 3
#define ICON_RAIN 4
#define ICON_SLEET 5
#define ICON_SNOW 6
#define ICON_FOG 7
#define ICON_HAIL 8
#define ICON_THUNDERSTORM 9
#define ICON_CLOUDY_NIGHT 10
#define ICON_CLEAR_NIGHT 11
#define ICON_WIND 12
#define ICON_TORNADO 13

namespace digitalcave {
    struct Daily {
        uint8_t icon;
        int8_t temperatureHigh;
        int8_t temperatureLow;
        uint32_t sunriseTime;
        uint32_t sunsetTime;
        float moonPhase;
    }

    struct Currently {
        uint8_t icon;
        float temperature;
        float dewPoint;
        float humidity;
        float cloudCover;
        float pressure;
        float windSpeed;
        float windGust;
        uint16_t windBearing;
        uint8_t uvIndex;
    }

    class Darksky {
    private:
        Webstream* web;

        char* latitude;
        char* longitude;
        char* apiKey;

        uint32_t time;          // the time that the data was refreshed
        Currently currently;    //
        Daily[3] daily;         // today, tomorrow, the next day

        uint8_t parse_icon(char* str);

    public:
        Darksky(Webstream* w, char* latitude, char* longitude, char* apiKey);
        ~Darksky();

        void update();
        Currently* currently();
        Daily* daily(uint8_t index);

    private:

    };
}

#endif