#ifndef STATE_H
#define STATE_H


namespace digitalcave {

    union E {
        char[32] darkskyApiKey;
        char[8] latitude;
        char[8] longitude;
    };

    class State {
    private:
        // transient
        uint8_t brightness;
        int8_t temperature;
        int8_t apparentTemperature;
        uint8_t weatherIcon;

        // eeprom

        int8_t zoneOffset;
        float latitude;
        float longitude;
        char[] darkskyApiKey;

        // rtc
        uint32_t time;

    public:
        uint8_t getBrightness();
        void setBrightness();

        char* getLatitude();
        char* getLongitude();
        char* getDarkskyApiKey();

        uint32_t getTime();
        void setTime(uint32_t time);
        int8_t getZoneOffset();
        setZoneOffset(int8_t offset);

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

    };
}

#endif