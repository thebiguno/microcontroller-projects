#include "Darksky.h"
#include "JsonParser.h"
#include <stdlib.h>
#include <string.h>

using namespace digitalcave;

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
    strcat(url, "?units=ca&exclude=minutely,hourly,alerts,flags");
    web->request((char*) "GET", url, (char*) NULL);
    web->header("Accept", "application/json");
    web->body("");

    char key[16];
    char value[20];
    uint8_t token;
    JsonParser parser = JsonParser(web, 64);
    while ((token = parser.nextToken()) != TOKEN_EOF) {
        if (token == TOKEN_KEY) {
            if (strcmp("currently", key) == 0) {
                while ((token = parser.nextToken()) != TOKEN_END_OBJECT) {
                    if (token == TOKEN_KEY) {
                        parser.getValue(key);
                        parser.nextToken();
                        if (strcmp("time", key) == 0) {
                            parser.getValue(value);
                            time = atol(value);
                        } else if (strcmp("icon", key) == 0) {
                            parser.getValue(value);
                            curr.icon = parse_icon(value);
                        } else if (strcmp("temperature", key) == 0) {
                            parser.getValue(value); // °C
                            curr.temperature = atof(value);
                        } else if (strcmp("dewPoint", key) == 0) {
                            parser.getValue(value); // °C
                            curr.dewPoint = atof(value);
                        } else if (strcmp("humidity", key) == 0) {
                            parser.getValue(value); // %
                            curr.humidity = atof(value);
                        } else if (strcmp("cloudCover", key) == 0) {
                            parser.getValue(value); // %
                            curr.humidity = atof(value);
                        } else if (strcmp("pressure", key) == 0) {
                            parser.getValue(value); // hPa
                            curr.pressure = atof(value);
                        } else if (strcmp("windSpeed", key) == 0) {
                            parser.getValue(value); // km/h
                            curr.windSpeed = atof(value);
                        } else if (strcmp("windGust", key) == 0) {
                            parser.getValue(value); // km/h
                            curr.windGust = atof(value);
                        } else if (strcmp("windBearing", key) == 0) {
                            parser.getValue(value); // degrees
                            curr.windBearing = atol(value);
                        } else if (strcmp("uvIndex", key) == 0) {
                            parser.getValue(value);
                            curr.uvIndex = atoi(value);
                        }
                    }
                }
            } else if (strcmp("daily", key) == 0) {
                for (uint8_t i = 0; i < 3; i++) {
                    while ((token = parser.nextToken()) != TOKEN_END_OBJECT) {
                        if (token == TOKEN_KEY) {
                            parser.getValue(key);
                            parser.nextToken();
                            if (strcmp("sunriseTime", key) == 0) {
                                parser.getValue(value); // unix time
                                days[i].sunriseTime = atol(value);
                            } else if (strcmp("sunsetTime", key) == 0) {
                                parser.getValue(value);
                                days[i].sunsetTime = atol(value);
                            } else if (strcmp("moonPhase", key) == 0) {
                                parser.getValue(value);
                                days[i].moonPhase = atof(value);
                            } else if (strcmp("temperatureLow", key) == 0) {
                                parser.getValue(value);
                                days[i].temperatureLow = atof(value);
                            } else if (strcmp("temperatureHigh", key) == 0) {
                                parser.getValue(value);
                                days[i].temperatureHigh = atof(value);
                            }
                        }
                    }
                }
            }
        }
    }
}

uint8_t Darksky::parse_icon(char* str) {
    if (strcmp("clear-day",str) == 0) return ICON_CLEAR_DAY;
    else if (strcmp("clear-night",str) == 0) return ICON_CLEAR_NIGHT;
    else if (strcmp("rain",str) == 0) return ICON_RAIN;
    else if (strcmp("snow",str) == 0) return ICON_SNOW;
    else if (strcmp("sleet",str) == 0) return ICON_SLEET;
    else if (strcmp("wind",str) == 0) return ICON_WIND;
    else if (strcmp("fog",str) == 0) return ICON_FOG;
    else if (strcmp("cloudy",str) == 0) return ICON_CLOUDY;
    else if (strcmp("partly-cloudy-day",str) == 0) return ICON_CLOUDY_DAY;
    else if (strcmp("partly-cloudy-night",str) == 0) return ICON_CLOUDY_NIGHT;
    else if (strcmp("hail",str) == 0) return ICON_HAIL;
    else if (strcmp("thunderstorm",str) == 0) return ICON_THUNDERSTORM;
    else if (strcmp("tornado",str) == 0) return ICON_TORNADO;
    else return ICON_UNKNOWN;
}

Currently Darksky::currently() {
    return this->curr;
}

Daily Darksky::daily(uint8_t index) {
    return this->days[index];
}
