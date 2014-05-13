#include "ds1307.h"
#include "../twi.h"

time_t t;

void get(struct time_t *t) {
	// register, unused...
	// seconds, minutes, hours, dow, date, month, year
	uint8_t data[7] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	
	twi_write_to(0xd0, data, 1, TWI_BLOCK, TWI_NO_STOP);
	twi_read_from(0xd0, data, 7, TWI_BLOCK, TWI_STOP);
	
	t->second = data[1] & 0x7f; // bit 7 is the enabled flag, always 0 for enabled
	t->minute = data[2];
	t->hour = data[3] & 0x3f;   // bit 6 is the 12/24 mode flag, always 0 for 24
	t->wday = data[4];
	t->day = data[5];
	t->month = data[6];
	t->year = data[7] + 2000;
	
	return t;
}

void set(struct time_t *t) {
	uint8_t data[8] = { 0x00, t.second, t.minute, t.hour, t.wday, t.day, t.month, t.year };
	twi_write_to(0xd0, data, 8, TWI_BLOCK, TWI_STOP); // write all the time registers
}