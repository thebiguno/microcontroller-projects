//This is meant to be used with RTC modules.  It gives you a calendar type, valid from 2000 to 2099.
//There are helper functions to increment calendar fields, switch between 12 and 24 hour modes, and
//get how many days are in a given month.

#ifndef TIME_H
#define TIME_H

#include <stdio.h>
#include <math.h>

#if defined (__cplusplus)
extern "C" {
#endif

#define TIME_MODE_AM			0
#define TIME_MODE_PM			1
#define TIME_MODE_24			2
#define TIME_MODE_12			3

#define TIME_FIELD_YEAR				0x00
#define TIME_FIELD_MONTH			0x01
#define TIME_FIELD_DAY_OF_MONTH		0x02
#define TIME_FIELD_HOUR				0x03
#define TIME_FIELD_MINUTE			0x04
#define TIME_FIELD_SECOND			0x05
#define TIME_FIELD_DAY_OF_WEEK		0x06

#define TIME_MONTH_JAN			1
#define TIME_MONTH_FEB			2
#define TIME_MONTH_MAR			3
#define TIME_MONTH_APR			4
#define TIME_MONTH_MAY			5
#define TIME_MONTH_JUN			6
#define TIME_MONTH_JUL			7
#define TIME_MONTH_AUG			8
#define TIME_MONTH_SEP			9
#define TIME_MONTH_OCT			10
#define TIME_MONTH_NOV			11
#define TIME_MONTH_DEC			12

typedef struct dc_time {
	uint16_t year;
	uint8_t month;
	uint8_t day_of_month;
	uint8_t day_of_week;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t mode;	//TIME_MODE_AM (0) = AM, TIME_MODE_PM (1) = PM, TIME_MODE_24 (2) = 24 hour
} dc_time_t;

//Increments (or decrements) the given field by the given amount.  If time_affects_date is 0, then time will just roll over without incrementing date; otherwise it will also change the date when rolling.
dc_time_t time_add(dc_time_t time, uint8_t field, int8_t amount, uint8_t time_affects_date);

//Sets mode to be 24 hour (TIME_MODE_24) or 12 hour (anything else).
dc_time_t time_set_mode(dc_time_t time, uint8_t mode);

//Returns the number of days in the given month.  Accounts correctly for leap years.
uint8_t time_get_days_in_month(uint16_t year, uint8_t month);

//Returns 1 if the year is a leap year, 0 otherwise
uint8_t time_is_leap_year(uint16_t year);

//Returns the day of the week for a given date.  0 = Sunday, 1 = Monday, ... 6 = Saturday
uint8_t time_get_day_of_week(dc_time_t time);


#if defined (__cplusplus)
}
#endif

#endif
