#include "ds1307.h"
#include "../../twi/twi.h"
#include "../../bcd/bcd.h"

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
	
	// it's not clear from the datasheet if the unused bits are 0 or undefined
	time->second = bcd2hex(data[1] & 0x7f);   // bit 7 is the CH flag
	time->minute = bcd2hex(data[2]);
	time->hour = bcd2hex(data[3]);
	time->wday = data[4];
	time->mday = bcd2hex(data[5]);
	time->month = bcd2hex(data[6]);
	time->year = 2000 + bcd2hex(data[7]);
	
	return t;
}

void ds1307_set(struct ds1307_time_t *t) {
	uint8_t data[8] = { 
		0x00,
		hex2bcd(t->second),
		hex2bcd(t->minute),
		hex2bcd(t->hour),
		hex2bcd(t->wday),
		hex2bcd(t->mday),
		hex2bcd(t->month),
		hex2bcd(t->year - 2000)
	};
	twi_write_to(0xd0, data, 8, TWI_BLOCK, TWI_STOP);
}
