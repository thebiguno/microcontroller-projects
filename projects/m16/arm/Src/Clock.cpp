#include "Clock.h"
#include "Matrix.h"
#include <Hsv.h>
#include <Rgb.h>
#include <stdlib.h>

using namespace digitalcave;

extern Hsv hsv;
extern Matrix matrix;

Clock::Clock() {
}

Clock::~Clock() {
}

void Clock::paint() {
/*
    if (frame > 0) {
        frame--;
    else {
        frame = 25; // repaint every 250 ms

        if (stale == 1) {
            readRtc(&date, &time);
        }

        matrix.setColor(0, 0, 0);
        matrix.rectangle(0, 0, 31, 31, DRAW_FILLED);

        for (uint8_t i = 0; i < 8; i++) {
            char c;
            switch (i) {
                case 0: c = time.Hours >> 4; break;
                case 1: c = time.Hours & 0xf; break;
                case 2: c = 10;
                case 3: c = time.Minutes >> 4; break;
                case 4: c = time.Minutes & 0xf; break;
            }
            matrix.character(i * 6, 0, 48 + c, 0);
        }

        for (uint8_t i = 0; i < 8; i++) {
            char c;
            switch (i) {
                case 3: c = date.Month >> 4; break;
                case 4: c = date.Month & 0xf; break;
                case 5: c = 253;
                case 6: c = date.Day >> 4; break;
                case 7: c = date.Day & 0xf; break;
            }
            matrix.character(i * 6, 8, 48 + c, 0);
        }

        switch (date.WeekDay) {
            case 1: matrix.character(0, 16, "MON", 0); break;
            case 2: matrix.character(0, 16, "TUE", 0); break;
            case 3: matrix.character(0, 16, "WED", 0); break;
            case 4: matrix.character(0, 16, "THU", 0); break;
            case 5: matrix.character(0, 16, "FRI", 0); break;
            case 6: matrix.character(0, 16, "SAT", 0); break;
            case 7: matrix.character(0, 16, "SUN", 0); break;
        }

        matrix.flush();
    }
*/
}
