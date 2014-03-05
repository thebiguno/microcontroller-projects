#ifndef TIME_H
#define TIME_H

#include <stdint.h>

struct time_t {
	uint16_t millis;
	uint8_t  second;
	uint8_t  minute;
	uint8_t  hour;
	uint8_t  day;
	uint8_t  month;
	uint16_t year;
	uint8_t  wday;
};

/*
 * Modifies the month, day, wday, and hour fields of the struct according to DST rules.
 * define TIME_DST_NEVER to disable (useful for zones that do not use DST)
 * define TIME_DST_EU to use Europeoan post-1996 rules
 * define TIME_DST_NA to use North American post-2007 rules
 * otherwise always adjusts the time (useful if you conditionally call this method and you have a physical switch)
 */
void time_summer(struct time_t *t);

/*
 * Turns a number of seconds since epoch into a time struct.
 * define TIME_FIRSTYEAR with your epoch year (default = 2000)
 * define TIME_FIRSTDAY with the offset of days from January 1 of your epoch to the first day of the week (default = 6)
 */
void time_get(uint32_t sec, struct time_t *t);

#endif

