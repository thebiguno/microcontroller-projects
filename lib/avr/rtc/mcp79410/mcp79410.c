#include "mcp79410.h"
#include "../../twi/twi.h"
#include "../../bcd/bcd.h"

void mcp79410_init() {
	uint8_t data[8] = { 0x07, 0x10 };  // clkout active at 1 Hz
	twi_write_to(0xde, data, 2, TWI_BLOCK, TWI_STOP);
	twi_write_to(0xde, data, 2, TWI_BLOCK, TWI_STOP);
}

void mcp79410_get(struct mcp79410_time_t *time) {
	// register, unused...
	// seconds, minutes, hours, dow, date, month, year
	uint8_t data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	
	twi_write_to(0xde, data, 1, TWI_BLOCK, TWI_NO_STOP);
	twi_read_from(0xde, data, 7, TWI_STOP);
	
	// it's not clear from the datasheet if the unused bits are 0 or undefined
	time->second = bcd2hex(data[1] & 0x7f);		// bit 7 is the ST flag
	time->minute = bcd2hex(data[2]);
	time->hour = bcd2hex(data[3]);
	time->wday = bcd2hex(data[4] & 0x07);		// bit 5 is OSCRUN, bit 4 is PWRFAIL, bit 3 is VBATEN
	time->mday = bcd2hex(data[5]);
	time->month = bcd2hex(data[6] & 0x1f);		// bit 5 is LPYR
	time->year = 2000 + bcd2hex(data[7]);
}

void mcp79410_set(struct mcp79410_time_t *time) {
	uint8_t data[8] = { 
		0x00,
		hex2bcd(time->second) | 0x80,		// bit 7 is ST
		hex2bcd(time->minute),
		hex2bcd(time->hour),
		hex2bcd(time->wday),				// bit 3 is VBATEN
		hex2bcd(time->mday),
		hex2bcd(time->month),				// bit 5 is LPYR
		hex2bcd(time->year - 2000)
	};
	twi_write_to(0xde, data, 8, TWI_BLOCK, TWI_STOP);
}

void mcp79410_set_alarm(uint8_t hour) {
	uint8_t data[8] = {
		0x0a,
		0x00,
		0x00,
		hex2bcd(hour),
		0x20,				// match on hour, assert with low level, clear match
		0x00,
		0x00
	};
	twi_write_to(0xde, data, 8, TWI_BLOCK, TWI_STOP);
}
