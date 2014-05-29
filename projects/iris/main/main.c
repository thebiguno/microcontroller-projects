#define WS2811_PORT PORTB

#include <avr/interrupt.h>
#include "lib/ws2811/ws2811_8.h"
//#include "lib/rtc/ds1307/ds1307.h"
#include "lib/rtc/pcf8563/pcf8563.h"
#include "lib/remote/remote.h"
#include "time32/time.h"
#include "time32/usa_dst.h"
#include "lib/serial/serial.h"
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
//	ws2811_t base03 = {.red = 0x00, .green = 0x1b, .blue = 0x26 };
	ws2811_t base02 = {.red = 0x07, .green = 0x26, .blue = 0x22 };		// background
//	ws2811_t base01 = {.red = 0x48, .green = 0x5e, .blue = 0x65 };
//	ws2811_t base00 = {.red = 0x55, .green = 0x6b, .blue = 0x73 };
//	ws2811_t base0 = {.red = 0x73, .green = 0x84, .blue = 0x86 };
	ws2811_t base1 = {.red = 0x83, .green = 0x91, .blue = 0x91 };		// foreground
	ws2811_t base2 = {.red = 0xde, .green = 0xd8, .blue = 0xc5 };		// markers
//	ws2811_t base3 = {.red = 0xed, .green = 0xe6, .blue = 0xd3 };
//	ws2811_t yellow = {.red = 0xa5, .green = 0x79, .blue = 0x00 };		// month summer
//	ws2811_t orange = {.red = 0xa5, .green = 0x3b, .blue = 0x06 };		// month autumn
//	ws2811_t red = {.red = 0xcc, .green = 0x22, .blue = 0x1f };
	ws2811_t magenta = {.red = 0xc3, .green = 0x26, .blue = 0x72 };
//	ws2811_t magenta_low = {.red = 0x0c, .green = 0x02, .blue = 0x07 };
	ws2811_t violet = {.red = 0x5c, .green = 0x61, .blue = 0xb4 };
//	ws2811_t violet_low = {.red = 0x05, .green = 0x06, .blue = 0x0b };
//	ws2811_t blue = {.red = 0x16, .green = 0x7b, .blue = 0xc2 };		// month winter
//	ws2811_t cyan = {.red = 0x1a, .green = 0x91, .blue = 0x88 };
//	ws2811_t green = {.red = 0x75, .green = 0x89, .blue = 0x00 };		// month spring
	
	struct ws2811_t colors[60];
	for (uint8_t i = 0; i < 60; i++) {
		colors[i] = black;
	}
	ws2811_set(colors, 60, 1);
	serial_init_b(9600);
	remote_init();
	
	EICRA |= _BV(ISC11);	// trigger on falling edge of int1
	EIMSK |= _BV(INT1);		// enable external interrupts on int1
	
	DDRD &= ~_BV(PD3);		// set 1 Hz int1 as input
	PORTD |= _BV(PD3);		// set 1 Hz int1 as tri-state
	
	pcf8563_init();

	int8_t mday = -1;		// used to avoid computing sunrise, sunset, and moon phase more than once per day;
	time_t prev_systime = 0;
	time_t systime;
	time_t risetime;
	time_t settime;
	struct tm sys;
	struct tm rise;
	struct tm set;
	int8_t phase = 0;	// the number of segments to light / 2
	uint8_t week = 0;
	uint8_t mode = 0;

	// hard coded location and time zone for now
	set_zone(-7 * ONE_HOUR);
	set_dst(usa_dst); // mountain
	set_position(183762, -410606); // calgary

	// TODO these values to come from RTC core registers
	sys.tm_year = 2014;
	sys.tm_mon = 4;
	sys.tm_mday = 18;
	sys.tm_hour = 8;
	sys.tm_min = 17;

	// read rtc (&sys);
	systime = mktime(&sys);
	set_system_time(systime);
	
	while (1) {
		;
		// 
		// 	// recompute time structure
		// 	prev_systime = systime;
		localtime_r(&__system_time, &sys);
		// 
		// 	if (sys.tm_mday != mday) {
		// 		// recompute sunrise, sunset, moon phase
		// 		mday = sys.tm_mday;
		// 		risetime = sun_rise(&systime);
		// 		settime = sun_set(&systime);
		// 		localtime_r(&risetime, &rise);
		// 		localtime_r(&settime, &set);
		// 		week = week_of_year(&sys, 0);
		// 		int8_t mp = moon_phase(&systime);
		// 		phase = 0.3 * mp;
		// 	}
		// 
		// 	// update display
		if (__system_time != systime) {
			systime = __system_time;
			struct ws2811_t fg = magenta_low;
			struct ws2811_t bg = violet_low;
			if (mode == 0) {
		// 		// hours, minutes, seconds
		// 		if (systime > risetime && systime < settime) {
		//			// invert display after sunset
		//			fg = base02;
		// 			bg = base2;
		// 		}
				for (uint8_t i = 0; i < 60; i++) {
					colors[i] = base02;
				}
				// markers
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = base2;
				}
				// hour fill
				uint8_t hour = (sys.tm_hour % 12) * 5;
				for (uint8_t i = hour + 1; i < hour + 5; i++) {
					colors[i] = base1;
				}
				colors[sys.tm_min] = violet;
				colors[sys.tm_sec] = magenta;
			} else if (mode == 1) {
				// month, day of month
				for (uint8_t i = 0; i < 60; i++) {
					colors[i] = base02;
				}
				//if (sys.tm_mon < 11) fg = orange;
				//if (sys.tm_mon < 8) fg = yellow;
				//if (sys.tm_mon < 5) fg = green;
				//if (sys.tm_mon < 2) fg = blue;
				// markers
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = base2;
				}
				// month fill
				uint8_t hour = (sys.tm_mon % 12) * 5;
				for (uint8_t i = hour + 1; i < hour + 5; i++) {
					colors[i] = base1;
				}
				// day fill
				if (sys.tm_mday < 31) {
					uint8_t i = sys.tm_mday * 2;
					colors[i] = violet;
					colors[i+1] = violet;
				} else {
					colors[58] = violet;
					colors[59] = violet;
					colors[0] = violet;
					colors[1] = violet;
				}
			} else if (mode == 2) {
				// week, day of week
				for (uint8_t i = 0; i < 60; i++) {
					colors[i] = black;
				}
				// markers
				for (uint8_t i = 0; i < 60; i = i + 8) {
					colors[i] = magenta_low;
				}
				// week fill
				uint8_t wday = sys.tm_wday * 8;
				for (uint8_t i = wday + 1; i < wday + 7; i++) {
					colors[i] = violet;
				}
				colors[week] = magenta;
		// 	} else if (mode == 3) {
		// 		// persoixante of day
		// 		uint8_t ps = ((sys.tm_hour * 60) + sys.tm_min) / 24;
		// 		uint8_t sr = ((rise.tm_hour * 60) + rise.tm_min) / 24;
		// 		uint8_t ss = ((set.tm_hour * 60) + set.tm_min) / 24;
		// 		for (uint8_t i = 0; i < ps; i ++) {
		// 			colors[i] = fg;
		// 		}
		// 		colors[sr] = magenta;
		// 		colors[ss] = violet;
		// 	} else if (mode == 4) {
		// 		// moon phase
		// 		for (uint8_t i = 0; i < 60; i++) {
		// 			colors[i] = bg;
		// 		}
		// 		if (phase > 0) {
		// 			// waxing
		// 			for (int8_t i = 15; i < 15 + phase; i++) {
		// 				colors[i] = fg;
		// 			}
		// 			for (int8_t i = 15; i > 15 - phase; i--) {
		// 				if (i < 0) i = 60 - i;
		// 				colors[i] = fg;
		// 			}
		// 		} else {
		// 			// waning
		// 			for (int8_t i = 45; i < 45 + phase; i++) {
		// 				colors[i] = fg;
		// 			}
		// 			for (int8_t i = 45; i > 45 - phase; i--) {
		// 				if (i > 60) i = i - 60;
		// 				colors[i] = fg;
		// 			}
			}

			// TODO translate the array
			ws2811_set(colors, 60, 1);
		}

		// 	// read ir
		uint8_t command = remote_get();
		if (command == REMOTE_LEFT) {
			UDR0 = 'L';
			if (mode > MODE_HMS) mode--;
		} else if (command == REMOTE_RIGHT) {
			UDR0 = 'R';
			if (mode < MODE_MOON) mode++;
		}
	}
}

ISR(INT1_vect, ISR_NAKED) {
	//UDR0 = 'T';
	system_tick();
	reti();
}
