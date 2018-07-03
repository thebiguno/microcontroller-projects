#include "Darksky.h"
#include <stdlib.h>

using namespace digitalcave;

extern State *state;

Darksky::Darksky(WebStream* w, char* lat, char* lon, char* key) :
web(w),
latitude(lat),
longitude(lon),
apiKey(key)
{}

Darksky::~Darksky() {
}

void Darksky::update() {
    char url[255];
    strcpy(url, "https://api.darksky.net/forecast/");
    strcat(url, apiKey);
    strcat(url, "/");
    strcat(url, latitude);
    strcat(url, ",");
    strcat(url, longitude);
    strcat(url, "?units=ca&exclude=minutely,hourly,alerts,flags")
    web.request("GET", url, NULL);
    web.header("Accept", "application/json");
    web.body("");

    char key[16];
    char value[20];
    uint8_t token;
    JsonParser parser = JsonParser(web);
    while ((token = parser.nextToken()) != TOKEN_EOF) {
        if (token == TOKEN_KEY) {
            if (strcmp("currently", key) == 0) {
                while ((token = parser.nextToken()) != TOKEN_END_OBJECT) {
                    if (token == TOKEN_KEY) {
                        parser.getValue(key);
                        parser.nextToken();
                        if (strcmp("time", key) == 0) {
                            parser.getValue(value); // unix time
                            time = atol(value);
                        } else if (strcmp("icon", key) == 0) {
                            // clear-day, clear-night, rain, snow, sleet, wind, fog, cloudy, partly-cloudy-day, partly-cloudy-night
                            // hail, thunderstorm, tornado
                            parser.getValue(value);
                            strcpy(icon, value)
                        } else if (strcmp("apparentTemperature", key) == 0) {
                            parser.getValue(value); // °C
                            temperature = atof(value);
                        } else if (strcmp("dewPoint"), key) == 0) {
                            parser.getValue(value); // °C
                            dewPoint = atof(value);
                        } else if (strcmp("humidity", key) == 0) {
                            parser.getValue(value); // %
                            humidity = atof(value);
                        } else if (strcmp("cloudCover", key) == 0) {
                            parser.getValue(value); // %
                            humidity = atof(value);
                        } else if (strcmp("pressure", key) == 0) {
                            parser.getValue(value); // hPa
                            pressure = atof(value);
                            pressure /= 10.0;        // kPa
                        } else if (strcmp("windSpeed", key) == 0) {
                            parser.getValue(value); // km/h
                            windSpeed = atof(value);
                        } else if (strcmp("windGust", key) == 0) {
                            parser.getValue(value); // km/h
                            windGust = atof(value);
                        } else if (strcmp("windBearing", key) == 0) {
                            parser.getValue(value); // degrees
                            windBearing = atol(value);
                        } else if (strcmp("uvIndex", key) == 0) {
                            parser.getValue(value);
                            uvIndex = atoi(value);
                        }
                    }
                }
            } else if (strcmp("daily", key) == 0) {
                // this will read only the first daily data object
                while ((token = parser.nextToken()) != TOKEN_END_OBJECT) {
                    if (token == TOKEN_KEY) {
                        parser.getValue(key);
                        parser.nextToken();
                        if (strcmp("sunriseTime", key) == 0) {
                            parser.getValue(value); // unix time
                            sunriseTime = atol(value);
                        } else if (strcmp("sunsetTime", key) == 0) {
                            parser.getValue(value);
                            sunsetTime = atol(value);
                        } else if (strcmp("moonPhase", key) == 0) {
                            parser.getValue(value);
                            moonPhase = atof(value);
                        } else if (strcmp("apparentTemperatureLow", key) == 0) {
                            parser.getValue(value);
                            temperatureLow = atof(value);
                        } else if (strcmp("apparentTemperatureHigh", key) == 0) {
                            parser.getValue(value);
                            temperatureLow = atof(value);
                        }
                    }
                }
            }
        }
    }
}