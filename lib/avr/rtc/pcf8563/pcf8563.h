#ifndef pcf8563_h
#define pcf8563_h

#include <stdint.h>

typedef struct pcf8563_t {
	uint8_t  second;	// 0 - 59
	uint8_t  minute;	// 0 - 59
	uint8_t  hour;		// 0 - 23
	uint8_t  mday;		// 1 - 31
	uint8_t  month;		// 1 - 12
	uint8_t  wday;		// 0 - 6; sunday = 0
	uint16_t year;		// 1900 - 2199
} pcf8563_time_t;

void pcf8563_init();
void pcf8563_get(struct pcf8563_t *time);
void pcf8563_set(struct pcf8563_t *time);

#endif
