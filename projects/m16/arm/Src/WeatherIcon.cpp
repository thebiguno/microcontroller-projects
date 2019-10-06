#include "WeatherIcon.h"
#include "Matrix.h"
#include <stdlib.h>
#include <string.h>

using namespace digitalcave;

extern Matrix matrix;

WeatherIcon::WeatherIcon(char* i) : icon(i) {
}

WeatherIcon::~WeatherIcon() {
}

void WeatherIcon::paint() {
    if (frame > 0) {
        frame--;
    } else {
        frame = 25; // repaint every 250 ms

        matrix.setColor(0, 0, 0);
        matrix.rectangle(0, 0, 31, 31, DRAW_FILLED);

        if (strcmp("clear-day", icon) == 0) {

        } else if (strcmp("clear-night", icon) ==0) {

        } else if (strcmp("rain", icon) ==0) {

        } else if (strcmp("snow", icon) ==0) {

        } else if (strcmp("sleet", icon) ==0) {

        } else if (strcmp("wind", icon) ==0) {

        } else if (strcmp("fog", icon) ==0) {

        } else if (strcmp("cloudy", icon) ==0) {

        } else if (strcmp("partly-cloudy-day", icon) ==0) {

        } else if (strcmp("partly-cloudy-night", icon) ==0) {

        } else if (strcmp("hail", icon) ==0) {

        } else if (strcmp("thunderstorm", icon) ==0) {

        } else if (strcmp("tornado", icon) ==0) {

        }

        matrix.flush();
    }
}
