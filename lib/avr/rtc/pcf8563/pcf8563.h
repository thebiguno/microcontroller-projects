#ifndef pcf8563_h
#define pcf8563_h

#include <stdint.h>

typedef struct pcf8563_time_t {
	uint8_t  second;
	uint8_t  minute;
	uint8_t  hour;
	uint8_t  mday;
	uint8_t  month;
	uint8_t  wday;
	uint16_t year;
} pcf8563_time_t;

void pcf8563_init();
void pcf8563_get(struct pcf8563_time_t *time);
void pcf8563_set(struct pcf8563_time_t *time);

#endif
