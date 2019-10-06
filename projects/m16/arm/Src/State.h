#ifndef STATE_H
#define STATE_H

#include <stdint.h>

namespace digitalcave {

    // eeprom data
    union E {
        int8_t zoneOffset;
        char darkskyApiKey[32];
        char latitude[8];
        char longitude[8];
    };

    class State {
    public:
        State();
        ~State();

        uint8_t getBrightness();
        void setBrightness();

        char* getLatitude();
        char* getLongitude();
        char* getDarkskyApiKey();

        uint32_t getTime();
        void setTime(uint32_t time);
        int8_t getZoneOffset();
        void setZoneOffset(int8_t offset);

        /* Read latitude, longitude, darksky API key, TZ offset from the eeprom */
        void readEeprom();
        /* Write latitude, longitude, darksky API key, TZ offset from the eeprom */
        void writeEeprom();

        /* Read the date/time from the RTC */
        void readRtc();
        /* Write the date/time to the RTC */
        void writeRtc();

        /* Read current time and weather data from darksky.net */
        void readDarksky();

    private:
        // transient
        uint8_t brightness;
        int8_t temperature;
        int8_t apparentTemperature;
        uint8_t weatherIcon;

        // rtc
        uint32_t time;
    };
}

#endif
