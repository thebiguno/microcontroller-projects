#include "dctime.h"

uint8_t time_is_leap_year(struct tm* tm){
	uint16_t year = tm->tm_year + 1900;
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

uint8_t time_days_in_month(struct tm* tm){
	uint8_t m = tm->tm_mon;
	if (m == 0 || m == 2 || m == 4 || m == 6 || m == 7 || m == 9 || m == 11){
		return 31;
	}
	else if (m == 3 || m == 5 || m == 8 || m == 10){
		return 30;
	}
	else if (m == 1){
		if (time_is_leap_year(tm)){
			return 29;
		}
		else {
			return 28;
		}
	}

	return 0xFF;
}
