#include "time.h"

/*
 * Author: Peter Dannegger <danni@specs.de>
 * http://www.mikrocontroller.net/attachment/8390/TIME.C51
 */

#ifndef TIME_FIRSTYEAR
#define TIME_FIRSTYEAR	2000		// start year
#endif

#ifndef TIME_FIRSTDAY
#define TIME_FIRSTDAY	6			// 0 = Sunday
#endif

uint8_t days[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

void time_summer(struct time_t *t) {
	uint8_t hour, day, wday, month;			// locals for faster access

	hour = t->hour;
	day = t->day;
	wday = t->wday;
	month = t->month;

#ifdef TIME_DST_NONE
	return;
#endif

#ifdef TIME_DST_EU
	// EU: from the last Sunday in March to the last Sunday in October
	if (month < 3 || month > 10) return;				// month 1, 2, 11, 12 -> Winter
	if (day - wday >= 25 && (wday || hour >= 2)) {		// after last Sunday 2:00
		if (month == 10) return; 						// October -> Winter
	} else {											// before last Sunday 2:00
		if (month == 3) return;							// March -> Winter
	}
#endif

#ifdef TIME_DST_NA
	// NA: from the second Sunday in March to the first Sunday in November
	if (month < 3 || month > 11) return;				// month 1, 2, 12 -> Winter
	if (day - wday >= 0 && (wday || hour >= 2)) {		// after first Sunday 2:00
		if (month == 11) return; 						// November -> Winter
	} else (day - wday >= 7 && (wday || hour >= 2)) {	// before last Sunday 2:00
		if (month == 3) return;							// March -> Winter
	}
#endif	

	hour++;												// add one hour
	if (hour == 24) {									// next day
		hour = 0;
		wday++;											// next weekday
		if (wday == 7) wday = 0;
		if (day == days[month-1]) {						// next month
			day = 0;
			month++;
		}
		day++;
	}
	t->month = month;
	t->hour = hour;
	t->day = day;
	t->wday = wday;
}


void time_get(uint32_t sec, struct time_t *t) {
	uint16_t day;
	uint8_t year;
	uint16_t dayofyear;
	uint8_t leap400;
	uint8_t month;

	t->second = sec % 60;
	sec /= 60;
	t->minute = sec % 60;
	sec /= 60;
	t->hour = sec % 24;
	day = sec / 24;

	t->wday = (day + TIME_FIRSTDAY) % 7;					// weekday

	year = TIME_FIRSTYEAR % 100;							// 0..99
	leap400 = 4 - ((TIME_FIRSTYEAR - 1) / 100 & 3);			// 4, 3, 2, 1

	for (;;){
		dayofyear = 365;
		if ((year & 3) == 0) {
			dayofyear = 366;								// leap year
			if (year == 0 || year == 100 || year == 200)	// 100 year exception
				if (--leap400)								// 400 year exception
					dayofyear = 365;
		}
		if (day < dayofyear) break;
		day -= dayofyear;
		year++;												// 00..136 / 99..235
	}
	t->year = year + TIME_FIRSTYEAR / 100 * 100;			// + century

	if (dayofyear & 1 && day > 58)							// no leap year and after 28.2.
		day++;												// skip 29.2.

	for (month = 1; day >= days[month-1]; month++)
		day -= days[month-1];

	t->month = month;										// 1..12
	t->day = day + 1;										// 1..31
}