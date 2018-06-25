#ifndef STATE_H
#define STATE_H

namespace digitalcave {
    class State {
    private:
        uint8_t brightness;
        float latitude;
        float longitude;
        char[] darkskyApiKey;
        int8_t zoneOffset;
        uint32_t sunrise;
        uint32_t sunset;
        int8_t temperature;
        int8_t apparentTemperature;
        uint8_t weatherIcon;

    public:
        uint8_t getBrightness();
        void setBrightness();

        float getLatitude();
        float getLongitude();
        char[] getDarkskyApiKey();

        uint32_t getTime();
        void setTime(uint32_t time);
        int8_t getZoneOffset();
        setZoneOffset(int8_t offset);

        uint32_t getSunrise();
        uint32_t getSunset();
        int8_t getTemperature();
        int8_t getApparentTemperature();

        /* 0: clear day
           1: clear night
           2: rain
           3: snow
           4: sleet
           5: wind
           6: fog
           7: cloudy
           8: partly cloudy day
           9: partly cloudy night
           10: hail
           11: thunderstorm
           12: tornado
         */
        uint8_t getWeatherIcon();

        /* Read latitude, longitude, darksky API key, TZ offset from the eeprom */
        void readEeprom();
        /* Write latitude, longitude, darksky API key, TZ offset from the eeprom */
        void writeEeprom();
        /* Read current time and weather data from darksky.net */
        void readDarksky();

    };
}

#endif