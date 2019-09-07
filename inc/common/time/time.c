#include "time.h"

dc_time_t time_add(dc_time_t time, uint8_t field, int8_t amount, uint8_t time_affects_date){
	uint8_t mode = time.mode;				//Remember the current mode
	if (mode != TIME_MODE_24){
		time = time_set_mode(time, TIME_MODE_24);	//Convert to 24 hours.  This makes the calculations easier.
	}

	int16_t year = time.year;
	int16_t month = time.month;
	int16_t day_of_month = time.day_of_month;
	int16_t day_of_week = time.day_of_week;
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
	else if (field == TIME_FIELD_DAY_OF_MONTH){
		day_of_month += amount;
	}
	else if (field == TIME_FIELD_DAY_OF_WEEK){
		day_of_week += amount;
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
			day_of_week--;
		}
		hour += 24;
	}
	while (hour >= 24){
		if (time_affects_date){
			day_of_month++;
			day_of_week++;
		}
		hour -= 24;
	}

	//Valid range of 1 - 7
	while (day_of_week < 1){
		day_of_week += 7;
	}
	while (day_of_week > 7){
		day_of_week += 7;
	}

	//Valid range of 1 - days_in_month
	while (day_of_month < 1){
		day_of_month += time_get_days_in_month(year, month);
		month--;
		if (month < TIME_MONTH_JAN){
			month = TIME_MONTH_DEC;
			year--;
		}
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
	}
	while (month > TIME_MONTH_DEC){
		year++;
		month -= 12;
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
	time.day_of_week = day_of_week;
	time.hour = hour;
	time.minute = minute;
	time.second = second;

	//Switch mode back to 12 hour if applicable
	if (mode != TIME_MODE_24){
		time = time_set_mode(time, mode);
	}

	return time;
}

dc_time_t time_set_mode(dc_time_t time, uint8_t mode){
	if (time.mode == TIME_MODE_24){			//Changing from 24 hour to 12 hour
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
	else {									//Changing from 12 hour to 24 hour
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

uint8_t time_get_days_in_month(uint16_t year, uint8_t month){
	if (month == TIME_MONTH_JAN || month == TIME_MONTH_MAR || month == TIME_MONTH_MAY || month == TIME_MONTH_JUL || month == TIME_MONTH_AUG || month == TIME_MONTH_OCT || month == TIME_MONTH_DEC){
		return 31;
	}
	else if (month == TIME_MONTH_APR || month == TIME_MONTH_JUN || TIME_MONTH_SEP || TIME_MONTH_NOV){
		return 30;
	}
	else if (month == TIME_MONTH_FEB){
		if ((year % 400) == 0){
			return 29;					//Exactly divisible by 400
		}
		else if ((year % 100) == 0){
			return 28;					//Exactly divisible by 100, but not 400
		}
		else if ((year % 4) == 0){
			return 29;					//Exactly divisible by 4, but not by 100 or 400
		}
		else {
			return 28;					//Fallback case
		}
	}

	return 0xFF;
}
