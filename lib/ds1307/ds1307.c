#include "ds1307.h"
#include "../twi.h"

struct tm _t;

void get(struct tm *t) {
	// register, unused...
	// seconds, minutes, hours, dow, date, month, year
	uint8_t data[7] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	
	twi_write_to(0xd0, data, 1, TWI_BLOCK, TWI_NO_STOP);
	twi_read_from(0xd0, data, 7, TWI_BLOCK, TWI_STOP);
	
	_t->second = data[1] & 0x7f; // bit 7 is the enabled flag, always 0 for enabled
	_t->minute = data[2];
	_t->hour = data[3] & 0x3f;   // bit 6 is the 12/24 mode flag, always 0 for 24
	_t->wday = data[4];
	_t->day = data[5];
	_t->month = data[6];
	_t->year = data[7] + 2000;
	
	return t;
}

void set(struct tm *t) {
	uint8_t data[8] = { 0x00, t->second, t->minute, t->hour, t->wday, t->day, t->month, t->year };
	twi_write_to(0xd0, data, 8, TWI_BLOCK, TWI_STOP); // write all the time registers
}
