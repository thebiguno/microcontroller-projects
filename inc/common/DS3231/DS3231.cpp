#include "DS3231.h"

using namespace digitalcave;

DS3231::DS3231(I2C* i2c) :
	i2c(i2c)
{
	uint8_t data[2];
	data[0] = 0x0E;		//Address for control register
	data[1] = 0x00;		//Set ~INT to output a 1Hz square wave
	I2CMessage message(data, sizeof(data));
	i2c->write(DS3231_ADDRESS, &message);
}

static inline uint8_t decimal_to_bcd(uint8_t decimal){
	return (decimal / 10) << 4 | (decimal % 10);
}

static inline uint8_t bcd_to_decimal(uint8_t bcd){
	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

void DS3231::get(tm_t* tm){
	uint8_t data[7];
	data[0] = 0x00;
	I2CMessage message(data, sizeof(data));
	message.setLength(1);
	i2c->write(DS3231_ADDRESS, &message);

	message.setLength(7);
	i2c->read(DS3231_ADDRESS, &message);

	//This struct expects year as 'years past 1900'.  The RTC provides it as century flag + 0-99, so we can put it in directly.
	tm->tm_year = bcd_to_decimal(data[6]) + (data[5] & (1 << 7) ? 100 : 0);

	//This struct expects month as 0 - 11.  The RTC provides it as 1 - 12, so we subtract 1.
	tm->tm_mon = bcd_to_decimal(data[5] & 0x1F) - 1;

	//The struct and RTC match on these two.
	tm->tm_mday = bcd_to_decimal(data[4]);
	tm->tm_wday = bcd_to_decimal(data[3]);

	//We assume that the clock is in 24 hour mode.  When we set it, we set it as such.
	tm->tm_hour = bcd_to_decimal(data[2]);
	tm->tm_min = bcd_to_decimal(data[1]);
	tm->tm_sec = bcd_to_decimal(data[0]);
}

void DS3231::set(tm_t* tm){
	uint8_t data[8];
	data[0] = 0x00;		//Register address
	data[1] = decimal_to_bcd(tm->tm_sec);
	data[2] = decimal_to_bcd(tm->tm_min);
	data[3] = decimal_to_bcd(tm->tm_hour);
	//Without setting the 12 hour flag, it is in 24 hour mode
	data[4] = decimal_to_bcd(tm->tm_wday);
	data[5] = decimal_to_bcd(tm->tm_mday);
	data[6] = decimal_to_bcd(tm->tm_mon + 1);
	uint8_t year = tm->tm_year;
	if (year >= 100){
		data[6] |= (1 << 7);	//Set the century flag
		year -= 100;
	}
	data[7] = decimal_to_bcd(year);
	I2CMessage message(data, sizeof(data));
	i2c->write(DS3231_ADDRESS, &message);
}
