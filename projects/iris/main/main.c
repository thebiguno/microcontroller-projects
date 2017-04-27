#define WS2811_PORT PORTB

#include <avr/interrupt.h>
#include "lib/ws281x/ws2812.h"
#include "lib/rtc/ds1307/ds1307.h"
#include "lib/twi/twi.h"
//#include "lib/serial/serial.h"
//#include "lib/rtc/pcf8563/pcf8563.h"
#include "lib/remote/remote.h"
#include "time32/time.h"
#include "time32/usa_dst.h"
#include <util/delay.h>
#include <math.h>

#define MODE_HMS 0
#define MODE_MD 1
#define MODE_WD 2
#define MODE_PCT 3
#define MODE_MOON 4
#define MODE_SOLID 5
#define MODE_SPECTRUM 6
#define MODE_PLASMA 7

#define MODE_YEAR 127
#define MODE_MONTH 128
#define MODE_DAY 129
#define MODE_HOUR 130
#define MODE_MIN 131
#define MODE_SEC 132

// a very bad random value generator
int r(int max) {
	return rand() % max;
}
// translate hue to rgb
void h2rgb(struct ws2812_t *rgb, float h) {
	h /= 60;		// sector 0 to 5
	int i = floor(h);
	float f = h - i;	// factorial part of h
	float q = 1 - f;
	float t = 1 - (1 - f);
	switch(i) {
		case 0:
		rgb->red = 255;
		rgb->green = 255 * t;
		rgb->blue = 0;
		break;
		case 1:
		rgb->red = 255 * q;
		rgb->green = 255;
		rgb->blue = 0;
		break;
		case 2:
		rgb->red = 0;
		rgb->green = 255;
		rgb->blue = 255 * t;
		break;
		case 3:
		rgb->red = 0;
		rgb->green = 255 * q;
		rgb->blue = 255;
		break;
		case 4:
		rgb->red = 255 * t;
		rgb->green = 0;
		rgb->blue = 255;
		break;
		default:
		rgb->red = 255;
		rgb->green = 0;
		rgb->blue = 255 * q;
		break;
	}
}
// recursive function to fill in the the spaces between two points in the plasma
void a(uint8_t p1, uint8_t p2, float *hues) {
	if (p1 == p2) return;

	uint8_t p = p1 + p2;
	if (p2 < p1) p += 60;
	p /= 2;
	if (p == p1 || p == p2) return;
	p %= 60;

	float v = hues[p1] + hues[p2];
	if (hues[p2] < hues[p1]) v += 360;
	v /= 2;
	if (v > 360) v -= 360;

	hues[p] = v;

	a(p1, p, hues);
	a(p, p2, hues);
}

// the complementary color index
inline uint8_t complementary(uint8_t c) {
	return (c + 6) % 12;
}

// the next triadic color index (clockwise)
inline uint8_t triad(uint8_t c) {
	return (c + 4) % 12;
}

inline uint8_t analagous_a(uint8_t c) {
	return (c + 1) % 12;
}
inline uint8_t analagous_b(uint8_t c) {
	return (c + 11) % 12;
}

int main() {

	// the rtc has a slow start up time from power on, so just delay a bit here
	_delay_ms(500);

	twi_init();

	DDRB |= _BV(2); // led strip output

	ws2812_t black = {.red = 0x00, .green = 0x00, .blue = 0x00 };
	ws2812_t white = {.red = 0xff, .green = 0xff, .blue = 0xff };

	struct ws2812_t palette[12];
	h2rgb(&palette[0],    0);	// red
	h2rgb(&palette[1],   30);	// orange
	h2rgb(&palette[2],   60);	// yellow
	h2rgb(&palette[3],   90);	// chartreuse
	h2rgb(&palette[4],  120);	// green
	h2rgb(&palette[5],  150);	// spring
	h2rgb(&palette[6],  180);	// cyan
	h2rgb(&palette[7],  210);	// azure
	h2rgb(&palette[8],  240);	// blue
	h2rgb(&palette[9],  270);	// violet
	h2rgb(&palette[10], 300);	// magenta
	h2rgb(&palette[11], 330);	// rose

	struct ws2812_t colors[60];

	// initialize hardware
	DDRB |= _BV(PB0);			// test output

	// INT1 / PD3 and Timer2 are used by the IR receiver
	remote_init(0x00);

	// Timer0 / T0 / PD4 is the 1 Hz square output of the RTC
	TCCR0B |= _BV(CS02) | _BV(CS01) | _BV(CS00);
	DDRD &= ~_BV(PD4);			// input
	PORTD |= _BV(PD4);			// RTC square wave is open drain and requires pull-up

	// Timer1 is used for animation
	TCCR1A = 0x00;
	TCCR1B |= _BV(CS12) | _BV(CS10); // clk / 1024 (about 1/10 second resolution)

	sei();

	_delay_ms(500);

	// Initialize serial
	//serial_init_b(9600);

	int8_t mday = -1;		// used to avoid computing sunrise, sunset, and moon phase more than once per day;
	time_t systime;
	time_t risetime;
	time_t settime;
	struct tm sys;
	struct tm rise;
	struct tm set;
	int8_t phase = 0;	// the number of segments to light / 2
	uint8_t week = 0;
	uint8_t mode = 0;
	uint8_t update = 1;
	struct ds1307_time_t rtc;
	uint8_t harmony = 0;

	// plasma variables
	float hues[60];
	int8_t p1 = 0;		// starting locations
	int8_t p2 = 20;
	int8_t p3 = 40;
	// start with red, green, blue
	hues[p1] = 0;
	hues[p2] = 120;
	hues[p3] = 240;

	uint16_t hue = 0;	// current solid hue

	// hard coded location and time zone for now
	set_zone(-7 * ONE_HOUR);
	set_dst(usa_dst); // mountain
	set_position(183762, -410606); // calgary

	// Initialize RTC chip
	ds1307_get(&rtc);
	ds1307_init();
	sys.tm_year = rtc.year - 1900;
	sys.tm_mon = rtc.month - 1;	// 0-11; rtc is 1 based; time lib is 0 based
	sys.tm_mday = rtc.mday;		// 1-31; rtc & time lib both 1 based
	sys.tm_hour = rtc.hour;		// 0-23
	sys.tm_min = rtc.minute;	// 0-59
	sys.tm_sec = rtc.second;	// 0-59
	systime = mktime(&sys);
	set_system_time(systime);
	ds1307_set(&rtc); // start the rtc

	uint8_t base_index = 0;

	struct ws2812_t base = black;
	struct ws2812_t primary = black;
	struct ws2812_t secondary = black;

	while (1) {
		if (TCNT0 > 0) {
			system_tick();
			TCNT0 = 0;
			update = 1;
		}

		if (mode == MODE_PLASMA && TCNT1 > 1024) {
			TCNT1 = 0x00;
			update = 1;
		}

		// update display
		// * if the update flag is set and the remote isn't in the middle of receiving a message
		if (update == 1 && remote_state() == 0) {
			update = 0;

			PORTB |= _BV(PB0);

			base = palette[base_index];
			if (harmony == 0) {
				// complementary
				primary = palette[complementary(base_index)];
				secondary = white;
			} else if (harmony == 1) {
				// split complementary
				primary = palette[analagous_a(complementary(base_index))];
				secondary = palette[analagous_b(complementary(base_index))];
			} else if (harmony == 2) {
				// triadic
				primary = palette[triad(base_index)];
				secondary = palette[triad(triad(base_index))];
			}

			if (mode <= MODE_MOON) {
				// recompute time structure
				time(&systime);
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
			}

			for (uint8_t i = 0; i < 60; i++) {
				colors[i] = black;
			}

			uint8_t blink = (sys.tm_sec & 0x01) == 0x00;
			if (mode == MODE_HMS) {
				// hours, minutes, seconds
				// markers for each hour, each 5 minutes
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = base;
				}
				uint8_t hour = (sys.tm_hour % 12) * 5;
				// replace one of the marker points with the hour hand (0-11)
				colors[hour] = primary;
				// for even numbered seconds, draw the minute hand (0-59)
				if (blink) colors[sys.tm_min] = secondary;
			} else if (mode == MODE_MD) {
				// month, day of month
				// markers for month, each 5 days
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = base;
				}
				// replace one of the marker points with the month hand (0-11)
				colors[sys.tm_mon * 5] = primary;
				// for even numbered seconds, draw the day of the month (0-30)
				if (blink) colors[sys.tm_mday - 1] = secondary;
			} else if (mode == MODE_WD) {
				// week of the year, day of week
				// markers for each 5 weeks, each day of the week
				colors[0] = base;
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = base;
				}
				// draw the week of the year (0 to 51)
				colors[week] = primary;
				// for even numbered seconds, replace one of the marker points with the day of the week
				if (blink) colors[sys.tm_wday * 5] = secondary;
			} else if (mode == MODE_PCT) {
				// persoixante of day
				uint8_t ps = ((sys.tm_hour * 60) + sys.tm_min) / 24;
				uint8_t sr = ((rise.tm_hour * 60) + rise.tm_min) / 24;
				uint8_t ss = ((set.tm_hour * 60) + set.tm_min) / 24;
				for (uint8_t i = 0; i < ps; i ++) {
					colors[i] = base;
				}
				colors[sr] = primary;
				colors[ss] = secondary;
			} else if (mode == MODE_MOON) {
				// moon phase
				if (phase > 0) {
					// waxing
					// 15 -> 45
					for (int8_t i = 14; i < 14 + phase; i++) {
						colors[i] = base;
					}

					for (int8_t i = 14; i > 14 - phase; i--) {
						colors[i < 0 ? i + 60 : i] = base;
					}
				} else {
					// waning
					for (int8_t i = 44; i < 44 - phase; i++) {
						colors[i > 60 ? i - 60 : i] = base;
					}
					for (int8_t i = 44; i > 44 + phase; i--) {
						colors[i] = base;
					}
				}
			} else if (mode == MODE_SPECTRUM) {
				for (uint8_t i = 0; i < 60; i++) {
					h2rgb(&colors[i], (hue + (i * 6)) % 360);
				}
				hue++;
				hue %= 360;
			} else if (mode == MODE_PLASMA) {
				a(p1, p2, hues);
				a(p2, p3, hues);
				a(p3, p1, hues);

				for (int8_t i = 0; i < 60; i++) {
					h2rgb(&colors[i], hues[i]);
				}
				/*
				colors[p1] = white;
				colors[p2] = white;
				colors[p3] = white;
				*/

				p1 += r(5) - 2;
				p2 += r(5) - 2;
				p3 += r(5) - 2;
				if (p1 < 0) p1 = 59;
				if (p2 < 0) p2 = 59;
				if (p3 < 0) p3 = 59;
				if (p1 > 59) p1 = 0;
				if (p2 > 50) p2 = 0;
				if (p3 > 59) p3 = 0;

				hues[p1] += (r(31) - 15);
				hues[p2] += (r(31) - 15);
				hues[p3] += (r(31) - 15);
				if (hues[p1] < 0) hues[p1] += 360;
				if (hues[p2] < 0) hues[p2] += 360;
				if (hues[p3] < 0) hues[p3] += 360;
				if (hues[p1] > 359) hues[p1] -= 360;
				if (hues[p2] > 359) hues[p2] -= 360;
				if (hues[p3] > 359) hues[p3] -= 360;
			} else if (mode == MODE_SOLID) {
				for (uint8_t i = 0; i < 60; i++) {
					h2rgb(&colors[i], hue);
				}
				hue++;
				hue %= 360;
			} else if (mode == MODE_YEAR) {
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = white;
				}
				colors[sys.tm_year] = palette[2]; // yellow
			} else if (mode == MODE_MONTH) {
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = white;
				}
				uint8_t mon = sys.tm_mon * 5;
				colors[mon] = palette[4]; // green
			} else if (mode == MODE_DAY) {
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = white;
				}
				colors[sys.tm_mday - 1] = palette[6]; // cyan
			} else if (mode == MODE_HOUR) {
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = white;
				}
				uint8_t hour = (sys.tm_hour % 12) * 5;
				colors[hour] = (sys.tm_hour >= 12) ? palette[8] : palette[2] ; // 0-12=blue : 13-23=yellow
			} else if (mode == MODE_MIN) {
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = white;
				}
				colors[sys.tm_min] = palette[10]; // magenta
			} else if (mode == MODE_SEC) {
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = white;
				}
				colors[sys.tm_sec] = palette[0];
			}

			struct ws2812_t tx[60];
			//for (int i = 0; i < 60; i++) tx[i] = colors[i];
			// translate the top to the bottom
			for (int i = 0; i < 60; i++) tx[i] = colors[(i + 30) % 60];
			ws281x_set(tx);
			remote_reset();

			PORTB &= ~_BV(PB0);
		}

		// read ir
		uint8_t command = remote_command();
		if (command >= REMOTE_MENU && command <= REMOTE_DOWN) update = 1;
		if (mode < MODE_YEAR) {
			if (command == REMOTE_LEFT) {
				if (mode > MODE_HMS) mode--;
			} else if (command == REMOTE_RIGHT) {
				if (mode < MODE_PLASMA) mode++;
			} else if (command == REMOTE_MENU) {
				mode = MODE_YEAR;
				time(&systime);
				localtime_r(&systime, &sys);
				sys.tm_year -= 100;
				if (sys.tm_year < 0) sys.tm_year = 0;
				if (sys.tm_year > 59) sys.tm_year = 59;
			} else if (command == REMOTE_UP) {
				base_index++;
				base_index %= 12;
			} else if (command == REMOTE_DOWN) {
				base_index--;
				if (base_index > 12) base_index = 11;
			} else if (command == REMOTE_CENTER) {
				harmony++;
				harmony %= 3;
			}
		} else {
			if (command == REMOTE_UP) {
				if (mode == MODE_YEAR && sys.tm_year < 59) sys.tm_year++;
				else if (mode == MODE_YEAR) sys.tm_year = 0;
				else if (mode == MODE_MONTH && sys.tm_mon < 11) sys.tm_mon++;
				else if (mode == MODE_MONTH) sys.tm_mon = 0;
				else if (mode == MODE_DAY && sys.tm_mday < 31) sys.tm_mday++;
				else if (mode == MODE_DAY) sys.tm_mday = 1;
				else if (mode == MODE_HOUR && sys.tm_hour < 23) sys.tm_hour++;
				else if (mode == MODE_HOUR) sys.tm_hour = 0;
				else if (mode == MODE_MIN && sys.tm_min < 59) sys.tm_min++;
				else if (mode == MODE_MIN) sys.tm_min = 0;
				else if (mode == MODE_SEC && sys.tm_sec < 59) sys.tm_sec++;
				else if (mode == MODE_SEC) sys.tm_sec = 0;
			} else if (command == REMOTE_DOWN) {
				if (mode == MODE_YEAR && sys.tm_year > 0) sys.tm_year--;
				else if (mode == MODE_YEAR) sys.tm_year = 59;
				else if (mode == MODE_MONTH && sys.tm_mon > 0) sys.tm_mon--;
				else if (mode == MODE_MONTH) sys.tm_mon = 11;
				else if (mode == MODE_DAY && sys.tm_mday > 1) sys.tm_mday--;
				else if (mode == MODE_DAY) sys.tm_mday = 31;
				else if (mode == MODE_HOUR && sys.tm_hour > 0) sys.tm_hour--;
				else if (mode == MODE_HOUR) sys.tm_hour = 23;
				else if (mode == MODE_MIN && sys.tm_min > 0) sys.tm_min--;
				else if (mode == MODE_MIN) sys.tm_min = 59;
				else if (mode == MODE_SEC && sys.tm_sec > 0) sys.tm_sec--;
				else if (mode == MODE_SEC) sys.tm_sec = 59;
			} else if (command == REMOTE_LEFT) {
				if (mode > MODE_YEAR) mode--;
			} else if (command == REMOTE_RIGHT) {
				if (mode < MODE_SEC) mode++;
				else {
					// set the new system time
					sys.tm_year += 100;
					systime = mktime(&sys);
					set_system_time(systime);

					// store the new system time to the rtc
					ds1307_init();
					ds1307_set(&rtc);
					mode = MODE_HMS;
				}
			} else if (command == REMOTE_MENU) {
				mode = MODE_HMS;
			}
		}
	}
}

EMPTY_INTERRUPT(TIMER0_OVF_vect)
