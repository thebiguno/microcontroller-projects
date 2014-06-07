#define WS2811_PORT PORTB

#include <avr/interrupt.h>
#include "lib/ws2811/ws2811_8.h"
//#include "lib/rtc/ds1307/ds1307.h"
#include "lib/serial/serial.h"
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
#define MODE_SPECTRUM 5
#define MODE_PLASMA 6

#define MODE_YEAR 127
#define MODE_MONTH 128
#define MODE_DAY 129
#define MODE_HOUR 130
#define MODE_MIN 131
#define MODE_SEC 132

extern time_t __system_time;

int r(int max) {
	return rand() % max;
}
void a(uint8_t p1, uint8_t p2, uint8_t *x) {
	if (p1 == p2) return;
	
	uint8_t p = p1 + p2;
	if (p2 < p1) p += 60;
	p /= 2;
	if (p == p1 || p == p2) return;
	p %= 60;
	
	uint8_t v = x[p1] + x[p2];
	if (x[p2] < x[p1]) v += 12;
	v /= 2;
	if (v < 0) v = 11;
	if (v > 11) v = 0;
	
	x[p] = v;
	
	a(p1, p, x);
	a(p, p2, x);
}

int main() {
	
	DDRB |= _BV(1); // led strip output
	
	ws2811_t black = {.red = 0x00, .green = 0x00, .blue = 0x00 };
	ws2811_t grey = {.red = 0xcc, .green = 0xcc, .blue = 0xcc };
	ws2811_t white = {.red = 0xff, .green = 0xff, .blue = 0xff };

	ws2811_t red = {.red = 0xcc, .green = 0x00, .blue = 0x00 };			// 0
	ws2811_t green = {.red = 0x00, .green = 0xcc, .blue = 0x00 };		// 120
	ws2811_t blue = {.red = 0x00, .green = 0x00, .blue = 0xcc };		// 240

	ws2811_t yellow = {.red = 0xcc, .green = 0x66, .blue = 0x00 };		// 60
	ws2811_t cyan = {.red = 0x00, .green = 0xcc, .blue = 0x66 };		// 180
	ws2811_t magenta = {.red = 0xcc, .green = 0x00, .blue = 0x66 };		// 300

	ws2811_t orange = {.red = 0xcc, .green = 0x66, .blue = 0x00 };		// 30
	ws2811_t chartreuse = {.red = 0x66, .green = 0xcc, .blue = 0x00 };	// 90
	ws2811_t spring = {.red = 0x00, .green = 0xcc, .blue = 0x66 };		// 150
	ws2811_t azure = {.red = 0x00, .green = 0x66, .blue = 0xcc };		// 210
	ws2811_t violet = {.red = 0x66, .green = 0x00, .blue = 0xcc };		// 270
	ws2811_t rose = {.red = 0xcc, .green = 0x00, .blue = 0x66 };		// 330

	ws2811_t palette[12] = { red, orange, yellow, chartreuse, green, spring, cyan, azure, blue, violet, magenta, rose };
	
	struct ws2811_t colors[60];

	// initialize hardware
	DDRB |= _BV(PB0);			// test output

	// INT1 / PD3 and Timer2 are used by the IR receiver
	remote_init();
	
	// Timer0 / T0 / PD4 is the 1 Hz square output of the RTC
	TCCR0B |= _BV(CS02) | _BV(CS01) | _BV(CS00);
	DDRD &= ~_BV(PD4);			// input
	PORTD |= _BV(PD4);			// RTC square wave is open drain and requires pull-up

	// Timer1 is used for animation
	TCCR1A = 0x00;
	TCCR1B |= _BV(CS12) | _BV(CS10); // clk / 1024 (about 1/10 second resolution)
	
	// Initialize RTC chip
	pcf8563_init();
	
	// Initialize serial
	serial_init_b(9600);
	
	sei();

	int8_t mday = -1;		// used to avoid computing sunrise, sunset, and moon phase more than once per day;
	time_t systime;
	time_t risetime;
	time_t settime;
	struct tm sys;
	struct tm rise;
	struct tm set;
	int8_t phase = 0;	// the number of segments to light / 2
	uint8_t year = 0;
	uint8_t week = 0;
	uint8_t mode = 0;
	uint8_t update = 1;
	struct pcf8563_t rtc;

	// plasma variables
	uint8_t x[60];
	int8_t p1 = 0;
	int8_t p2 = 20;
	int8_t p3 = 40;
	x[p1] = 0;
	x[p2] = 4;
	x[p3] = 8;

	// hard coded location and time zone for now
	set_zone(-7 * ONE_HOUR);
	set_dst(usa_dst); // mountain
	set_position(183762, -410606); // calgary

	pcf8563_get(&rtc);
	sys.tm_year = rtc.year;
	sys.tm_mon = rtc.month - 1;
	sys.tm_mday = rtc.mday;
	sys.tm_hour = rtc.hour;
	sys.tm_min = rtc.minute;
	systime = mktime(&sys);
	set_system_time(systime);
	
	struct ws2811_t markers = chartreuse;
	struct ws2811_t fill = violet;
	struct ws2811_t pixel = blue;
	
	while (1) {
		if (TCNT0 > 0) {
			system_tick();
			TCNT0 = 0;
			update = 1;
		}
		
		if (mode == MODE_PLASMA && TCNT1 > 0x400) {
			TCNT1 = 0x00;
			update = 1;
		}
		
		// update display
		// * if the update flag is set and the remote isn't in the middle of receiving a message
		if (update == 1 && remote_state() == 0) {
			update = 0;
			PORTB |= _BV(PB0);
			if (mode <= MODE_MOON) {
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
					
					phase = 0.3 * mp;
				}

				if (systime < risetime || systime > settime) {
					// invert display after sunset
					markers = violet;
					fill = yellow;
					pixel = green;
				} else {
					markers = chartreuse;
					fill = blue;
					pixel = magenta;
				}
			}
			
			// almost all of the modes start with black
			for (uint8_t i = 0; i < 60; i++) {
				colors[i] = black;
			}

			if (mode == MODE_HMS) {
				// hours, minutes, seconds
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
				colors[sys.tm_sec] = red;
			} else if (mode == MODE_MD) {
				// month, day of month
				if (sys.tm_mon < 2) fill = azure; // jan, feb
				else if (sys.tm_mon < 5) fill = spring; // mar, apr, may
				else if (sys.tm_mon < 8) fill = rose; // jun, jul, aug
				else if (sys.tm_mon < 11) fill = orange; // sep, oct, nov
				else fill = azure; // dec
				
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
			} else if (mode == MODE_WD) {
				// week, day of week
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
			} else if (mode == MODE_PCT) {
				// persoixante of day
				uint8_t ps = ((sys.tm_hour * 60) + sys.tm_min) / 24;
				uint8_t sr = ((rise.tm_hour * 60) + rise.tm_min) / 24;
				uint8_t ss = ((set.tm_hour * 60) + set.tm_min) / 24;
				for (uint8_t i = 0; i < ps; i ++) {
					colors[i] = fill;
				}
				colors[sr] = yellow;
				colors[ss] = magenta;
			} else if (mode == MODE_MOON) {
				// moon phase
				if (phase > 0) {
					// waxing
					for (int8_t i = 15 - phase; i < 15 + phase; i++) {
						if (i < 0) i = 60 + i;
						colors[i] = azure;
					}
				} else {
					// waning
					for (int8_t i = 45 - phase; i < 45 + phase; i++) {
						if (i > 60) i = i - 60;
						colors[i] = azure;
					}
				}
			} else if (mode == MODE_SPECTRUM) {
				for (uint8_t i = 0; i < 60; i++) {
					if (i < 5) colors[i] = red;
					else if (i < 10) colors[i] = orange;
					else if (i < 15) colors[i] = yellow;
					else if (i < 20) colors[i] = chartreuse;
					else if (i < 25) colors[i] = green;
					else if (i < 30) colors[i] = spring;
					else if (i < 35) colors[i] = cyan;
					else if (i < 40) colors[i] = azure;
					else if (i < 45) colors[i] = blue;
					else if (i < 50) colors[i] = violet;
					else if (i < 55) colors[i] = magenta;
					else if (i < 60) colors[i] = rose;
				}
			} else if (mode == MODE_PLASMA) {
				a(p1, p2, x);
				a(p2, p3, x);
				a(p3, p1, x);

				for (int8_t i = 0; i < 60; i++) {
					colors[i] = palette[x[i]];
				}
				p1 += r(3) - 1;
				p2 += r(3) - 1;
				p3 += r(3) - 1;
				if (p1 < 0) p1 = 19;
				if (p2 < 20) p2 = 39;
				if (p3 < 40) p3 = 59;
				if (p1 > 19) p1 = 0;
				if (p2 > 39) p2 = 20;
				if (p3 > 59) p3 = 40;
				x[p1] += (r(3) - 1);
				x[p2] += (r(3) - 1);
				x[p3] += (r(3) - 1);
				if (x[p1] < 0) x[p1] = 0;
				if (p2 < 20) p2 = 20;
				if (p3 < 40) p3 = 40;
				if (p1 > 19) p1 = 19;
				if (p2 > 39) p2 = 39;
				if (p3 > 59) p3 = 59;
			} else if (mode == MODE_YEAR) {
				colors[year] = yellow;
			} else if (mode == MODE_MONTH) {
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = markers;
				}
				uint8_t mon = (sys.tm_mon % 12) * 5;
				for (uint8_t i = mon + 1; i < mon + 5; i++) {
					colors[i] = green;
				}
			} else if (mode == MODE_DAY) {
				if (sys.tm_mday < 31) {
					uint8_t i = sys.tm_mday * 2;
					colors[i] = cyan;
					colors[i+1] = cyan;
				} else {
					colors[58] = cyan;
					colors[59] = cyan;
					colors[0] = cyan;
					colors[1] = cyan;
				}
			} else if (mode == MODE_HOUR) {
				if (sys.tm_hour > 11) {
					markers = chartreuse;
				} else {
					markers = violet;
				}
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = markers;
				}
				uint8_t hour = (sys.tm_hour % 12) * 5;
				for (uint8_t i = hour + 1; i < hour + 5; i++) {
					colors[i] = blue;
				}
			} else if (mode == MODE_MIN) {
				colors[sys.tm_min] = magenta;
			} else if (mode == MODE_SEC) {
				colors[sys.tm_sec] = red;
			}

			// TODO translate the array
			ws2811_set(colors, 60, 1);
			ws2811_set(colors, 1, 1);
			remote_reset();

			PORTB &= ~_BV(PB0);
		}

		// read ir
		uint8_t command = remote_get();
		if (command >= REMOTE_MENU && command <= REMOTE_DOWN) update = 1;
		if (mode <= MODE_PLASMA) {
			if (command == REMOTE_LEFT) {
				if (mode > MODE_HMS) mode--;
			} else if (command == REMOTE_RIGHT) {
				if (mode < MODE_PLASMA) mode++;
			} else if (command == REMOTE_MENU) {
				mode = MODE_YEAR;
				year = 2000 - sys.tm_year;
				if (year > 59) year = 0;
			}
		} else {
			if (command == REMOTE_UP) {
				if (mode == MODE_YEAR && year < 59) year++;
				else if (mode == MODE_YEAR) year = 0;
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
				if (mode == MODE_YEAR && year > 0) year--;
				else if (mode == MODE_YEAR) year = 59;
				else if (mode == MODE_MONTH && sys.tm_mon > 0) sys.tm_mon--;
				else if (mode == MODE_MONTH) sys.tm_mon = 11;
				else if (mode == MODE_DAY && sys.tm_mday > 0) sys.tm_mday--;
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
					systime = mktime(&sys);
					set_system_time(systime);
					rtc.year = 2000 + year;
					rtc.month = sys.tm_mon + 1;		// rtc is 1 based; time lib is 0 based
					rtc.mday = sys.tm_mday;
					rtc.hour = sys.tm_hour;
					rtc.minute = sys.tm_min;
					rtc.second = sys.tm_sec;
					pcf8563_set(&rtc);
					mode = MODE_HMS;
					
					// this just for testing
					pcf8563_get(&rtc);
					sys.tm_year = rtc.year;
					sys.tm_mon = rtc.month - 1;
					sys.tm_mday = rtc.mday;
					sys.tm_hour = 0; //rtc.hour;
					sys.tm_min = 0; //rtc.minute;
					sys.tm_sec = rtc.second;
					systime = mktime(&sys);
					set_system_time(systime);
				}
			} else if (command == REMOTE_MENU) {
				mode = MODE_HMS;
			}
		}
	}
}

EMPTY_INTERRUPT(TIMER0_OVF_vect)
