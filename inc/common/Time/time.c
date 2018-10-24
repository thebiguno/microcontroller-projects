#include "time.h"

#include <stdio.h>

// see: http://howardhinnant.github.io/date_algorithms.html
// magic numbers explained
// there are 719468 days from 0000-03-01 to 1970-01-01
// there are 146097 days in an era
// there are 36524 days in the first hundred years of an era
// there are 86400 seconds in a day

int64_t mktime(const struct tm* t) {
    int16_t y = 1900 + t->tm_year;
    uint8_t m = t->tm_mon + 1;
    uint8_t d = t->tm_mday;

    y -= (m <= 2);
    int8_t era = (y >= 0 ? y : y - 399) / 400;                // [-2, 5]
    uint16_t yoe = y - era * 400;                             // [0, 399]
    uint16_t doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d-1;  // [0, 365]
    uint32_t doe = yoe*365 + yoe/4 - yoe/100 + doy;           // [0, 146096]

    return (era * 146097 + static_cast<int64_t>(doe) - 719468) * 86400 + (t->tm_hour * 3600) + (t->tm_min * 60) + t->tm_sec;
}

tm gmtime(int64_t z) {
    uint32_t tmp = z % 86400;

    tm t;
    t.tm_hour = tmp / 3600;
    tmp -= t.tm_hour * 3600;
    t.tm_min = tmp / 60;
    tmp -= t.tm_min * 60;
    t.tm_sec = tmp;

    z /= 86400;  // seconds to days
    z += 719468; // 1970-01-01 to 0000-03-01

    int8_t era = z / 146097;                                         // [-2, 5]
    uint32_t doe = z - era * 146097;                                 // [0, 146096]
    uint16_t yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
    int16_t y = yoe + era * 400;
    uint16_t doy = doe - (365 * yoe + yoe / 4 - yoe / 100);          // [0, 365]
    uint8_t mp = (5 * doy + 2) / 153;                                // [0, 11]

    t.tm_mday = 1 + doy - (153 * mp + 2) / 5;                        // [1, 31]
    t.tm_mon = mp + (mp < 10 ? 3 : -9) - 1;                          // [0, 11]

    t.tm_year = (y + (t.tm_mon <= 2)) - 1900;
    t.tm_wday = z >= -4 ? (z+3) % 7 : (z+4) % 7 + 6;                 // [0, 6]
    t.tm_yday = doy + (mp < 10 ? 59 : -306);

    return t;
}
