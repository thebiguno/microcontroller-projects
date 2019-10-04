#include "time.h"

dc_time_t time_add(dc_time_t time, uint8_t field, int8_t amount, uint8_t time_affects_date){
	uint8_t mode = time.mode;				//Remember the current mode
	if (mode != TIME_MODE_24){
		time = time_set_mode(time, TIME_MODE_24);	//Convert to 24 hours.  This makes the calculations easier.
	}

	int16_t year = time.year;
	int16_t month = time.month;
	int16_t day_of_month = time.day_of_month;
	int16_t hour = time.hour;
	int16_t minute = time.minute;
	int16_t second = time.second;

	//Increment fields
	if (field == TIME_FIELD_YEAR){
		year += amount;
	}
	else if (field == TIME_FIELD_MONTH){
		month += amount;
	}
	else if (field == TIME_FIELD_DAY_OF_MONTH || field == TIME_FIELD_DAY_OF_WEEK){
		day_of_month += amount;
	}
	else if (field == TIME_FIELD_HOUR){
		hour += amount;
	}
	else if (field == TIME_FIELD_MINUTE){
		minute += amount;
	}
	else if (field == TIME_FIELD_SECOND){
		second += amount;
	}

	//Account for rollovers.  Start at seconds so that the rollovers can accumulate (e.g. 2010-12-31 23:59:59 plus 1 second is 2011-01-01 00:00:00)

	//Valid range of 0-59
	while (second < 0){
		minute--;
		second += 60;
	}
	while (second >= 60){
		minute++;
		second -= 60;
	}

	//Valid range of 0-59
	while (minute < 0){
		hour--;
		minute += 60;
	}
	while (minute >= 60){
		hour++;
		minute -= 60;
	}

	//Valid range of 0-23
	while (hour < 0){
		if (time_affects_date){
			day_of_month--;
		}
		hour += 24;
	}
	while (hour >= 24){
		if (time_affects_date){
			day_of_month++;
		}
		hour -= 24;
	}

	//Valid range of 1 - days_in_month
	while (day_of_month < 1){
		month--;
		if (month < TIME_MONTH_JAN){
			month = TIME_MONTH_DEC;
			year--;
		}
		day_of_month += time_get_days_in_month(year, month);
	}
	while (day_of_month > time_get_days_in_month(year, month)){
		day_of_month -= time_get_days_in_month(year, month);
		month++;
		if (month > TIME_MONTH_DEC){
			month = TIME_MONTH_JAN;
			year++;
		}
	}

	//Valid range of 1 - 12
	while (month < TIME_MONTH_JAN){
		year--;
		month += 12;
		while (day_of_month > time_get_days_in_month(year, month)){
			day_of_month--;
		}
	}
	while (month > TIME_MONTH_DEC){
		year++;
		month -= 12;
		while (day_of_month > time_get_days_in_month(year, month)){
			day_of_month--;
		}
	}

	//Valid range of 2000 - 2099
	if (year < 2000){
		year = 2000;
	}
	else if (year >= 2100){
		year = 2099;
	}

	time.year = year;
	time.month = month;
	time.day_of_month = day_of_month;
	time.hour = hour;
	time.minute = minute;
	time.second = second;

	//Switch mode back to 12 hour if applicable
	if (mode != TIME_MODE_24){
		time = time_set_mode(time, mode);
	}

	//Calculate the day of week
	time.day_of_week = time_get_day_of_week(time);

	return time;
}

dc_time_t time_set_mode(dc_time_t time, uint8_t mode){
	if (mode != TIME_MODE_24 && time.mode == TIME_MODE_24){			//Changing from 24 hour to 12 hour
		if (time.hour == 0){				//Midnight
			time.hour = 12;
			time.mode = TIME_MODE_AM;
		}
		else if (time.hour > 12){			//PM
			time.hour = time.hour - 12;
			time.mode = TIME_MODE_PM;
		}
		else {								//AM
			time.mode = TIME_MODE_AM;
		}
	}
	else if (mode == TIME_MODE_24 && time.mode != TIME_MODE_24) {	//Changing from 12 hour to 24 hour
		if (time.hour == 12){				//Noon or midnight are special cases; subtract 12 (and if it is PM, later re-add 12)
			time.hour = 0;
		}

		if (time.mode == TIME_MODE_PM){
			time.hour = time.hour + 12;		//Add 12 hours if it is PM
		}
		time.mode = TIME_MODE_24;
	}

	return time;
}

uint8_t time_is_leap_year(uint16_t year){
	if ((year % 400) == 0){
		return 1;					//Exactly divisible by 400
	}
	else if ((year % 100) == 0){
		return 0;					//Exactly divisible by 100, but not 400
	}
	else if ((year % 4) == 0){
		return 1;					//Exactly divisible by 4, but not by 100 or 400
	}
	else {
		return 0;					//Fallback case
	}

}

uint8_t time_get_days_in_month(uint16_t year, uint8_t month){
	if (month == TIME_MONTH_JAN || month == TIME_MONTH_MAR || month == TIME_MONTH_MAY || month == TIME_MONTH_JUL || month == TIME_MONTH_AUG || month == TIME_MONTH_OCT || month == TIME_MONTH_DEC){
		return 31;
	}
	else if (month == TIME_MONTH_APR || month == TIME_MONTH_JUN || month == TIME_MONTH_SEP || month == TIME_MONTH_NOV){
		return 30;
	}
	else if (month == TIME_MONTH_FEB){
		if (time_is_leap_year(year)){
			return 29;
		}
		else {
			return 28;
		}
	}

	return 0xFF;
}

//We use the Key Value method to calculate day of week.
uint8_t time_get_day_of_week(dc_time_t time){
	int16_t t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
	time.year -= time.month < 3;
	return ( time.year + time.year / 4 - time.year / 100 + time.year / 400 + t[time.month - 1] + time.day_of_month) % 7;
	// //Months start at March = 1, April = 2, ... December = 10, January = 11, February = 12.
	// int8_t month_number = time.month - 2;
	// uint8_t year = time.year;
	// if (month_number <= 0){	//
	// 	month_number += 12;
	// 	year--;
	// }
	// return (time.day_of_month + (int16_t) floor((13 * month_number - 1) / 5) + year + (int16_t) floor(year / 4) + (int16_t) floor(20 / 4) - 2 * 20) % 7;
}

uint8_t time_after(dc_time_t a, dc_time_t b){
	a = time_set_mode(a, TIME_MODE_24);
	b = time_set_mode(b, TIME_MODE_24);

	if (a.hour > b.hour || (a.hour == b.hour && a.minute > b.minute) || (a.hour == b.hour && a.minute == b.minute && a.second > b.second)){
		return 1;
	}
	return 0;
}

uint8_t time_before(dc_time_t a, dc_time_t b){
	a = time_set_mode(a, TIME_MODE_24);
	b = time_set_mode(b, TIME_MODE_24);

	if (a.hour < b.hour || (a.hour == b.hour && a.minute < b.minute) || (a.hour == b.hour && a.minute == b.minute && a.second < b.second)){
		return 1;
	}
	return 0;
}
