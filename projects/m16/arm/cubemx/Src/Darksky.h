#ifndef DARKSKY_H
#define DARKSKY_H

#include <stdint.h>

namespace digitalcave {
    class Darksky {
    private:
        Webstream* web;

        char* latitude;
        char* longitude;
        char* apiKey;


        // current data

        // the time that the data was refreshed
        uint32_t time;
        char icon[16];
        // temperatures in degrees Celsius
        float temperature;
        float dewPoint;
        // percentage values
        float humidity;
        float cloudCover;
        // pressure in kPa
        float pressure;
        // wind speeds in km/h
        float windSpeed;
        float windGust;
        // wind direction in degrees;
        uint16_t windBearing;
        // UV index
        uint8_t uvIndex;

        // daily data

        uint32_t sunriseTime;
        uint32_t sunsetTime;
        float temperatureHigh;
        float temperatureLow;
        float moonPhase;


    public:
        Darksky(Webstream* w, char* latitude, char* longitude, char* apiKey);
        ~Darksky();
        void update();
    private:

    };
}

#endif