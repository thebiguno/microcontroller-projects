#include <avr/interrupt.h>
#include "lib/ws2811/ws2811.h"
//#include "lib/ds1307/ds1307.h"
#include "lib/remote/remote.h"
#include "time32/time.h"
#include "time32/usa_dst.h"
//#include "lib/serial/serial.h"

int main() {
	
//	ws2811_t base03 = {.red = 2, .green = 43, .blue = 54 };			// background
	ws2811_t base02 = {.red = 6, .green = 54, .blue = 66 };
//	ws2811_t base01 = {.red = 88, .green = 110, .blue = 117 };
//	ws2811_t base00 = {.red = 101, .green = 123, .blue = 131 };
//	ws2811_t base0 = {.red = 131, .green = 148, .blue = 150 };
	ws2811_t base1 = {.red = 147, .green = 161, .blue = 161 };
	ws2811_t base2 = {.red = 238, .green = 232, .blue = 213 };		// foreground
//	ws2811_t base3 = {.red = 253, .green = 246, .blue = 227 };
	ws2811_t yellow = {.red = 181, .green = 137, .blue = 24 };		// month summer
	ws2811_t orange = {.red = 203, .green = 75, .blue = 22 };		// month autumn
//	ws2811_t red = {.red = 220, .green = 49, .blue = 47 };
	ws2811_t magenta = {.red = 211, .green = 54, .blue = 130 };
	ws2811_t violet = {.red = 108, .green = 113, .blue = 196 };
	ws2811_t blue = {.red = 38, .green = 139, .blue = 210 };		// month winter
//	ws2811_t cyan = {.red = 42, .green = 161, .blue = 152 };
	ws2811_t green = {.red = 133, .green = 153, .blue = 29 };		// month spring
//	ws2811_t white = {.red = 255, .green = 255, .blue = 255 };
//	ws2811_t black = {.red = 0, .green = 0, .blue = 0 };

	int8_t mday = -1;					// used to avoid computing sunrise, sunset, and moon phase more than once per day;
	time_t prev_systime = 0;
	time_t systime;
	time_t risetime;
	time_t settime;
	struct tm sys;
	struct tm rise;
	struct tm set;
	int8_t phase = 0;	// the number of segments to light / 2
	uint8_t week = 0;
	struct ws2811_t colors[60];
	uint8_t mode = 0;

	// TODO these values to come from RTC core registers
	sys.tm_year = 2014;
	sys.tm_mon = 4;
	sys.tm_mday = 18;
	sys.tm_hour = 8;
	sys.tm_min = 17;

	// TODO these values to come from RTC gp registers
	set_zone(-7 * ONE_HOUR);
	set_dst(usa_dst); // mountain
	set_position(183762, -410606); // calgary


	// read rtc (&sys);
	systime = mktime(&sys);
	set_system_time(systime);
	while (1) {
		if (prev_systime < systime) {

			// recompute time structure
			prev_systime = systime;
			localtime_r(&systime, &sys);

			if (sys.tm_mday != mday) {
				// recompute sunrise, sunset, moon phase
				mday = sys.tm_mday;
				risetime = sun_rise(&systime);
				settime = sun_set(&systime);
				localtime_r(&risetime, &rise);
				localtime_r(&settime, &set);
				week = week_of_year(&sys, 0);
				int8_t mp = moon_phase(&systime);
				phase = 0.3 * mp;
			}

			// update display
			struct ws2811_t fg = base2;
			struct ws2811_t bg = base02;
			if (mode == 0) {
				// hours, minutes, seconds
				if (systime > risetime && systime < settime) {
					// invert display after sunset
					fg = base02;
					bg = base2;
				}
				for (uint8_t i = 0; i < 60; i++) {
					colors[i] = bg;
				}
				// markers
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = base1;
				}
				// hour fill
				uint8_t hour = (sys.tm_hour % 12) * 5;
				for (uint8_t i = hour + 1; i < hour + 6; i++) {
					colors[i] = fg;
				}
				colors[sys.tm_min] = violet;
				colors[sys.tm_sec] = magenta;
			} else if (mode == 1) {
				// month, day of month
				for (uint8_t i = 0; i < 60; i++) {
					colors[i] = bg;
				}
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = base1;
				}
				struct ws2811_t season = blue;
				if (sys.tm_mon < 11) season = orange;
				if (sys.tm_mon < 8) season = yellow;
				if (sys.tm_mon < 5) season = green;
				if (sys.tm_mon < 2) season = blue;
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = season;
				}
				if (sys.tm_mday < 31) {
					// the 31st is not shown
					uint8_t i = sys.tm_mday * 2;
					colors[i] = violet;
					colors[i+1] = violet;
				}
			} else if (mode == 2) {
				// week, day of week
				for (uint8_t i = 0; i < 60; i++) {
					colors[i] = bg;
				}
				for (uint8_t i = 0; i < 60; i = i + 8) {
					colors[i] = base1;
				}
				// week fill
				uint8_t wday = sys.tm_wday * 8;
				for (uint8_t i = wday + 1; i < wday + 7; i++) {
					colors[i] = fg;
				}
				colors[week] = violet;
			} else if (mode == 3) {
				// persoixante of day
				uint8_t ps = ((sys.tm_hour * 60) + sys.tm_min) / 24;
				uint8_t sr = ((rise.tm_hour * 60) + rise.tm_min) / 24;
				uint8_t ss = ((set.tm_hour * 60) + set.tm_min) / 24;
				for (uint8_t i = 0; i < ps; i ++) {
					colors[i] = fg;
				}
				colors[sr] = magenta;
				colors[ss] = violet;
			} else if (mode == 4) {
				// moon phase
				for (uint8_t i = 0; i < 60; i = i + 15) {
					colors[i] = base02;
				}
				if (phase > 0) {
					// waxing
					for (int8_t i = 15; i < 15 + phase; i++) {
						colors[i] = fg;
					}
					for (int8_t i = 15; i > 15 - phase; i--) {
						if (i < 0) i = 60 - i;
						colors[i] = fg;
					}
				} else {
					// waning
					for (int8_t i = 45; i < 45 + phase; i++) {
						colors[i] = fg;
					}
					for (int8_t i = 45; i > 45 - phase; i--) {
						if (i > 60) i = i - 60;
						colors[i] = fg;
					}
				}
			}
			
			ws2811_set(colors, 60);

			// read ir
			uint8_t command = remote_get();
			if (command == REMOTE_LEFT) {
				mode--;
				if (mode < 0) mode = 4;
			} else if (command == REMOTE_RIGHT) {
				mode++;
				if (mode > 4) mode = 0;
			}
		}
	}
}

ISR(INT1_vect, ISR_NAKED) {
	system_tick();
	reti();
}
