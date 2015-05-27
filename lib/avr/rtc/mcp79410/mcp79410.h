#ifndef mcp79410_h
#define mcp79410_h

#include <stdint.h>

struct mcp79410_time_t {
	uint8_t  second;
	uint8_t  minute;
	uint8_t  hour;
	uint8_t  wday;
	uint8_t  mday;
	uint8_t  month;
	uint8_t  year;
};

void mcp79410_init();
void mcp79410_get(struct mcp79410_time_t *time);
void mcp79410_set(struct mcp79410_time_t *time);

#endif
