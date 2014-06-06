#include "pcf8563.h"
#include "../../twi/twi.h"
#include "../../bcd/bcd.h"

void pcf8563_init() {
	// register, control 1, control 2
	uint8_t data[8] = { 0x0d, 0x83, 0x00 };
	twi_write_to(0x51, data, 2, TWI_BLOCK, TWI_STOP);
	data[0] = 0x00;
	data[1] = 0x00;
	twi_write_to(0x51, data, 3, TWI_BLOCK, TWI_STOP);
}

void pcf8563_get(struct pcf8563_t *time) {
	// register, unused...
	// seconds, minutes, hours, days, weekdays, century_months, years
	uint8_t data[7] = { 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	
	twi_write_to(0x51, data, 1, TWI_BLOCK, TWI_NO_STOP);
	twi_read_from(0x51, data, 7, TWI_STOP);
	
	time->second = bcd2hex(data[0] & 0x7f);   // bit 7 is the VL flag
	time->minute = bcd2hex(data[1]);
	time->hour = bcd2hex(data[2]);
	time->mday = bcd2hex(data[3]);
	time->wday = data[4];
	time->month = bcd2hex(data[5] & 0x7f);    // bit 7 is century
	time->year = (((data[5] & 0x80) * 100) + 1900) + bcd2hex(data[6]);
}

void pcf8563_set(struct pcf8563_t *t) {
	uint8_t cy = t->year - 1900;
	uint8_t c = cy / 100;
	uint8_t y = cy - (c * 100);
	uint8_t data[8] = { 
		0x02, 
		hex2bcd(t->second),
		hex2bcd(t->minute),
		hex2bcd(t->hour),
		hex2bcd(t->mday),
		t->wday,
		(c << 7) & hex2bcd(t->month),    // bit 7 is century
		hex2bcd(y)
	};
	twi_write_to(0x51, data, 8, TWI_BLOCK, TWI_STOP);
}
