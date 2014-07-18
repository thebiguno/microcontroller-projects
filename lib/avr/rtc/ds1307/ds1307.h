#ifndef ds1307_h
#define ds1307_h

#include <stdint.h>

struct ds1307_time_t {
	uint8_t  second;
	uint8_t  minute;
	uint8_t  hour;
	uint8_t  wday;
	uint8_t  mday;
	uint8_t  month;
	uint8_t  year;
};

void ds1307_init();
void ds1307_get(struct ds1307_time_t *time);
void ds1307_set(struct ds1307_time_t *time);

#endif
