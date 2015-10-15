#include "mcp79410.h"
#include "twi.h"
#include "bcd.h"

#ifdef __cplusplus
extern "C" {
#endif

void mcp79410_get(struct mcp79410_time_t *time) {
	// register, unused...
	// seconds, minutes, hours, dow, date, month, year
	uint8_t data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	
	twi_write_to(0x6f, data, 1, TWI_BLOCK, TWI_NO_STOP);
	twi_read_from(0x6f, data, 7, TWI_STOP);
	
	// it's not clear from the datasheet if the unused bits are 0 or undefined
	time->second = bcd2hex(data[0] & 0x7f);		// bit 7 is the ST flag
	time->minute = bcd2hex(data[1]);
	time->hour = bcd2hex(data[2]);
	time->wday = bcd2hex(data[3] & 0x07);		// bit 5 is OSCRUN, bit 4 is PWRFAIL, bit 3 is VBATEN
	time->mday = bcd2hex(data[4]);
	time->month = bcd2hex(data[5] & 0x1f);		// bit 5 is LPYR
	time->year = 2000 + bcd2hex(data[6]);
}

void mcp79410_set(struct mcp79410_time_t *time) {
	// see the note in section 5.3 about how to set the timekeeping registers without rollover issues
	uint8_t data[7];

	// disable the crystal input (ST = 0)
	data[0] = 0x00;								// seconds register
	data[1] = 0x00;								// ST = 0
	twi_write_to(0x6f, data, 2, TWI_BLOCK, TWI_STOP);

	// wait for the oscilator to stop (OSCRUN == 0)
	data[0] = 0x03;
	data[1] = 0x20;
	while (data[1] & 0x20) {
		twi_read_from(0x6f, data, 2, TWI_STOP);
	}
	
	// set the time fields
	data[0] = 0x01;								// minutes register
	data[1] = hex2bcd(time->minute);
	data[2] = hex2bcd(time->hour);
	data[3] = hex2bcd(time->wday) | 0x08,		// enable battery (VBATEN = 1) 
	data[4] = hex2bcd(time->mday),
	data[5] = hex2bcd(time->month),				// bit 5 is LPYR
	data[6] = hex2bcd(time->year - 2000);
	twi_write_to(0x6f, data, 7, TWI_BLOCK, TWI_STOP);
	
	// set the seconds field and enable the crystal input (ST = 1)
	data[0] = 0x00;								// seconds register
	data[1] = hex2bcd(time->second) | 0x80;
	twi_write_to(0x6f, data, 2, TWI_BLOCK, TWI_STOP);
}

inline void mcp79410_control(uint8_t control) {
	uint8_t data[2] = { 0x07, control };
	twi_write_to(0x6f, data, 2, TWI_BLOCK, TWI_STOP);
}

void mcp79410_set_mfp_low() {
	mcp79410_control(0x00);
}	
void mcp79410_set_mfp_high() {
	mcp79410_control(0x80);
}	
void mcp79410_set_mfp_1hz() {
	mcp79410_control(0x40);
}
void mcp79410_set_mfp_4khz() {
	mcp79410_control(0x41);
}
void mcp79410_set_mfp_8khz() {
	mcp79410_control(0x42);
}
void mcp79410_set_mfp_32khz() {
	mcp79410_control(0x43);
}

uint8_t mcp79410_read_sram(uint8_t offset) {
	uint8_t data[1];
	data[0] = 0x20 + offset;
	twi_write_to(0x6f, data, 1, TWI_BLOCK, TWI_NO_STOP);
	twi_read_from(0x6f, data, 1, TWI_STOP);
	return data[0];
}

void mcp79410_write_sram(uint8_t b, uint8_t offset) {
	uint8_t data[2] = { 0, b };
	data[0] = 0x20 + offset;
	twi_write_to(0x6f, data, 2, TWI_BLOCK, TWI_STOP);
}

void mcp79410_set_trim(int8_t trim) {
	uint8_t data[2] = { 0x09, trim };
	twi_write_to(0x6f, data, 2, TWI_BLOCK, TWI_STOP);
}

#ifdef __cplusplus
}
#endif
