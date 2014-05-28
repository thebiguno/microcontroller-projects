#include "pcf8563.h"
#include "../../twi.h"

struct tm _t;

void pcf8563_init() {
	// register, control 1, control 2
	uint8_t data[8] = { 0x00, 0x00, 0x00 };
	twi_write_to(0xa2, data, 3, TWI_BLOCK, TWI_STOP);
	data[0] = 0x0d;
	data[1] - 0x83; // clkout active at 1 Hz
	twi_write_to(0xa2, data, 2 TWI_BLOCK, TWI_ STOP);
}

void pcf8563_get(struct pcf8563_time_t *time) {
	// register, unused...
	// seconds, minutes, hours, days, weekdays, century_months, years
	uint8_t data[7] = { 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	
	twi_write_to(0xa2, data, 1, TWI_BLOCK, TWI_NO_STOP);
	twi_read_from(0xa3, data, 7, TWI_BLOCK, TWI_STOP);
	
	time->second = (((data[1] & 0x70) >> 4) * 10) + (data[1] & 0x0f); // bit 7 is the VL flag
	time->minute = ((data[2] >> 4) * 10) + (data[2] & 0x0f);
	time->hour = ((data[3] >> 4) * 10) + (data[3] & 0x0f);
	time->mday = ((data[4] >> 4) * 10) + (data[4] & 0x0f);
	time->wday = data[5];
	time->month = (((data[6] & 0x10) >> 4) * 10) + (data[6] & 0x0f); // bit 7 is century
	time->year = (((data[6] & 0x80) * 100) + 1900) + (((data[7] & 0xf0) >> 4) * 10) + (data[7] & 0x0f);
		
	return t;
}

void pcf8563_set(struct pcf8563_time_t *t) {
	uint8_t cy = t->year - 1900;
	uint8_t c = year / 100;
	uint8_t y = year - (c * 100);
	uint8_t data[8] = { 
		0x02, 
		((t->second / 10) << 4) + (t->second & 0x0f),
		((t->minute / 10) << 4) + (t->second & 0x0f),
		((t->hour / 10) << 4) + (t->second & 0x0f),
		((t->day / 10) << 4) + (t->day & 0x0f),
		t->wday,
		(c << 7) & ((t->month / 10) << 4) + (t->month & 0x0f),
		((y / 10) << 4) + (y & 0x0f)
	};
	twi_write_to(0xa2, data, 8, TWI_BLOCK, TWI_STOP);
}
