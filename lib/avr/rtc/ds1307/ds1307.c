#include "ds1307.h"
#include "../../twi.h"

void ds1307_init() {
	uint8_t data[8] = { 0x07, 0x10 };  // clkout active at 1 Hz
	twi_write_to(0xa2, data, 2, TWI_BLOCK, TWI_STOP);
}

void ds1307_get(struct ds1307_time_t *t) {
	// register, unused...
	// seconds, minutes, hours, dow, date, month, year
	uint8_t data[7] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	
	twi_write_to(0xd0, data, 1, TWI_BLOCK, TWI_NO_STOP);
	twi_read_from(0xd0, data, 7, TWI_BLOCK, TWI_STOP);
	
	time->second = (((data[1] & 0x70) >> 4) * 10) + (data[1] & 0x0f); // bit 7 is the enabled flag
	time->minute = ((data[2] >> 4) * 10) + (data[2] & 0x0f);
	time->hour = ((data[3] >> 4) * 10) + (data[3] & 0x0f);
	time->wday = data[5];
	time->mday = ((data[4] >> 4) * 10) + (data[4] & 0x0f);
	time->month = (((data[6] & 0x10) >> 4) * 10) + (data[6] & 0x0f); // bit 7 is century
	time->year = 2000 + (((data[7] & 0xf0) >> 4) * 10) + (data[7] & 0x0f);
	
	return t;
}

void ds1307_set(struct ds1307_time_t *t) {
	uint8_t y = year - 2000;
	uint8_t data[8] = { 
		0x00,
		((t->second / 10) << 4) + (t->second & 0x0f),
		((t->minute / 10) << 4) + (t->second & 0x0f),
		((t->hour / 10) << 4) + (t->second & 0x0f),
		((t->day / 10) << 4) + (t->day & 0x0f),
		t->wday,
		(c << 7) & ((t->month / 10) << 4) + (t->month & 0x0f),
		((y / 10) << 4) + (y & 0x0f)
	};
	twi_write_to(0xd0, data, 8, TWI_BLOCK, TWI_STOP);
}
