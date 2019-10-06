// Some helper methods which are not included in time.h

#ifndef DC_TIME_H
#define DC_TIME_H

#include <stdio.h>
#include <math.h>
#include <time.h>

#if defined (__cplusplus)
extern "C" {
#endif


//Returns the number of days in the given month.  Accounts correctly for leap years.
uint8_t time_days_in_month(struct tm* tm);

//Returns 1 if the year is a leap year, 0 otherwise
uint8_t time_is_leap_year(struct tm* tm);

#if defined (__cplusplus)
}
#endif

#endif
