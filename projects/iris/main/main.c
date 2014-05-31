#define WS2811_PORT PORTB

#include <avr/interrupt.h>
#include "lib/ws2811/ws2811_8.h"
//#include "lib/rtc/ds1307/ds1307.h"
#include "lib/rtc/pcf8563/pcf8563.h"
#include "lib/remote/remote.h"
#include "time32/time.h"
#include "time32/usa_dst.h"
#include <util/delay.h>

#define MODE_HMS 0
#define MODE_MD 1
#define MODE_WD 2
#define MODE_PCT 3
#define MODE_MOON 4

#define MODE_YEAR 127
#define MODE_MONTH 128
#define MODE_DAY 129
#define MODE_HOUR 130
#define MODE_MIN 131
#define MODE_SEC 132

extern time_t __system_time;

int main() {
	
	DDRB |= _BV(1); // led strip output
	
	ws2811_t black = {.red = 0x00, .green = 0x00, .blue = 0x00 };
	ws2811_t yellow = {.red = 0xcc, .green = 0xcc, .blue = 0x00 };
	ws2811_t orange = {.red = 0xcc, .green = 0x33, .blue = 0x00 };
	ws2811_t red = {.red = 0xff, .green = 0x00, .blue = 0x00 };
	ws2811_t magenta = {.red = 0x99, .green = 0x00, .blue = 0x99 };
	ws2811_t violet = {.red = 0x11, .green = 0x11, .blue = 0x66 };
	ws2811_t blue = {.red = 0x00, .green = 0x00, .blue = 0xff };
	ws2811_t cyan = {.red = 0x00, .green = 0x99, .blue = 0x99 };
	ws2811_t green = {.red = 0x00, .green = 0xff, .blue = 0x00 };
	ws2811_t base02 = {.red = 0x11, .green = 0x11, .blue = 0x11 };
	ws2811_t base2 = {.red = 0x99, .green = 0x99, .blue = 0x99 };

	
	struct ws2811_t colors[60];
	remote_init();
	
	EICRA |= _BV(ISC11);	// trigger on falling edge of int1
	EIMSK |= _BV(INT1);		// enable external interrupts on int1
	
	DDRD &= ~_BV(PD3);		// set 1 Hz int1 as input
	PORTD |= _BV(PD3);		// set 1 Hz int1 as pull-up
	
	pcf8563_init();
	
	sei();

	int8_t mday = -1;		// used to avoid computing sunrise, sunset, and moon phase more than once per day;
	volatile time_t systime;
	time_t risetime;
	time_t settime;
	struct tm sys;
	struct tm rise;
	struct tm set;
	int8_t phase = 0;	// the number of segments to light / 2
	uint8_t week = 0;
	uint8_t mode = 0;
	uint8_t update = 1;

	// hard coded location and time zone for now
	set_zone(-7 * ONE_HOUR);
	set_dst(usa_dst); // mountain
	set_position(183762, -410606); // calgary

	// TODO these values to come from RTC core registers
	sys.tm_year = 2014;
	sys.tm_mon = 4;
	sys.tm_mday = 18;
	sys.tm_hour = 14;
	sys.tm_min = 17;

	// read rtc (&sys);
	systime = mktime(&sys);
	set_system_time(systime);
	
	while (1) {
		// update display
		if (__system_time > systime || update > 0) {
			// recompute time structure
			localtime_r(&__system_time, &sys);

			if (sys.tm_mday != mday) {
				// recompute sunrise, sunset, moon phase
				mday = sys.tm_mday;
				risetime = sun_rise(&systime);
				settime = sun_set(&systime);
				localtime_r(&risetime, &rise);
				localtime_r(&settime, &set);
				week = week_of_year(&sys, 0);
				int8_t mp = moon_phase(&systime);
				phase = 15; //0.3 * mp;
			}
			
			update = 0;
			systime = __system_time;
			struct ws2811_t markers = base02;
			struct ws2811_t bg = black;
			struct ws2811_t fill = violet;
			struct ws2811_t pixel = blue;
			if (mode == 0) {
				// hours, minutes, seconds
				if (systime < risetime || systime > settime) {
					// invert display after sunset
					markers = magenta;
				}
				for (uint8_t i = 0; i < 60; i++) {
					colors[i] = bg;
				}
				// markers
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = markers;
				}
				// hour fill
				uint8_t hour = (sys.tm_hour % 12) * 5;
				for (uint8_t i = hour + 1; i < hour + 5; i++) {
					colors[i] = fill;
				}
				colors[sys.tm_min] = pixel;
				colors[sys.tm_sec] = yellow;
			} else if (mode == 1) {
				// month, day of month
				for (uint8_t i = 0; i < 60; i++) {
					colors[i] = bg;
				}
				if (sys.tm_mon < 11) fill = orange;
				if (sys.tm_mon < 8) fill = yellow;
				if (sys.tm_mon < 5) fill = green;
				if (sys.tm_mon < 2) fill = blue;
				
				// markers
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = markers;
				}
				// month fill
				uint8_t mon = (sys.tm_mon % 12) * 5;
				for (uint8_t i = mon + 1; i < mon + 5; i++) {
					colors[i] = fill;
				}
				// day fill
				if (sys.tm_mday < 31) {
					uint8_t i = sys.tm_mday * 2;
					colors[i] = pixel;
					colors[i+1] = pixel;
				} else {
					colors[58] = pixel;
					colors[59] = pixel;
					colors[0] = pixel;
					colors[1] = pixel;
				}
			} else if (mode == 2) {
				// week, day of week
				for (uint8_t i = 0; i < 60; i++) {
					colors[i] = bg;
				}
				// markers
				for (uint8_t i = 0; i < 60; i = i + 8) {
					colors[i] = markers;
				}
				// week fill
				uint8_t wday = sys.tm_wday * 8;
				for (uint8_t i = wday + 1; i < wday + 7; i++) {
					colors[i] = fill;
				}
				colors[week] = pixel;
			} else if (mode == 3) {
				// persoixante of day
				for (uint8_t i = 0; i < 60; i++) {
					colors[i] = bg;
				}
				uint8_t ps = ((sys.tm_hour * 60) + sys.tm_min) / 24;
				uint8_t sr = ((rise.tm_hour * 60) + rise.tm_min) / 24;
				uint8_t ss = ((set.tm_hour * 60) + set.tm_min) / 24;
				for (uint8_t i = 0; i < ps; i ++) {
					colors[i] = fill;
				}
				colors[sr] = yellow;
				colors[ss] = magenta;
			} else if (mode == 4) {
				// moon phase
				for (uint8_t i = 0; i < 60; i++) {
					colors[i] = bg;
				}
				if (phase > 0) {
					// waxing
					for (int8_t i = 15; i < 15 + phase; i++) {
						colors[i] = base02;
					}
					for (int8_t i = 15; i > 15 - phase; i--) {
						if (i < 0) i = 60 - i;
						colors[i] = base02;
					}
				} else {
					// waning
					for (int8_t i = 45; i < 45 + phase; i++) {
						colors[i] = base02;
					}
					for (int8_t i = 45; i > 45 - phase; i--) {
						if (i > 60) i = i - 60;
						colors[i] = base02;
					}
				}
			}

			// TODO translate the array
			ws2811_set(colors, 60, 1);
		}

		// 	// read ir
		uint8_t command = remote_get();
		if (command == REMOTE_LEFT) {
			if (mode > MODE_HMS) mode--;
			update = 1;
		} else if (command == REMOTE_RIGHT) {
			if (mode < MODE_MOON) mode++;
			update = 1;
		}
	}
}

ISR(INT1_vect, ISR_NAKED) {
	system_tick();
	reti();
}
