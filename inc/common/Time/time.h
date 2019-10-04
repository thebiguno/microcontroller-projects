#ifndef TIME_H
#include <stdlib.h>
#include <stdint.h>

struct tm {
    uint8_t  tm_sec;    // 0-59
    uint8_t  tm_min;    // 0-59
    uint8_t  tm_hour;   // 0-23
    uint8_t  tm_mday;   // 0-31
    uint8_t  tm_mon;    // 0-11
    int16_t  tm_year;   // years since 1900
    uint8_t  tm_wday;   // 0-6 (Sun-Sat)
    uint16_t tm_yday;   // 0-365
    uint8_t  tm_isdst;  // 0-1
};

int64_t mktime(const struct tm* t);
struct tm gmtime(int64_t z);

#endif
