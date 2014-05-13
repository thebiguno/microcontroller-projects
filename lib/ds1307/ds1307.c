#include "ds1307.h"
#include "../i2c.h"

time_t t;

time_t get() {
	// device/write, register, unused...
	// device/read, seconds, minutes, hours, dow, date, month, year
	uint8_t msg = { 0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	
	i2c_start_transceiver_with_data(msg, 2); // position the register position to 0
	msg[0] = 0xd1;
	i2c_start_transceiver_with_data(msg, 8); // read all of the time registers
	
	t.second = msg[1] & 0x7f; // bit 7 is the enabled flag, always 1
	t.minute = msg[2];
	t.hour = msg[3] & 0x7f;   // bit 6 is the 12/24 mode flag, always 0 for 24
	t.wday = msg[4];
	t.day = msg[5];
	t.month = msg[6];
	t.year = msg[7] + 2000;
	
	return t;
}

void set(time_t t) {
	uint8_t msg = { 0xd0, 0x00, t.second, t.minute, t.hour, t.wday, t.day, t.month, t.year };
	i2c_start_transceiver_with_data(msg, 9); // write all the time registers
}