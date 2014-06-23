#define WS2811_PORT PORTB

#include <avr/interrupt.h>
#include "lib/ws2811/ws2811_w.h"
//#include "lib/rtc/ds1307/ds1307.h"
#include "lib/serial/serial.h"
#include "lib/rtc/pcf8563/pcf8563.h"
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
#define MODE_SPECTRUM 5
#define MODE_PLASMA 6
#define MODE_SOLID 7

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
void h2rgb(struct ws2811_t *rgb, float h) {
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
	return (c + 12) % 12;
}

// the next triadic color index (clockwise)
inline uint8_t triad(uint8_t c) {
	return (c + 8) % 12;
}

inline uint8_t analagous_a(uint8_t c) {
	return (c + 2) % 12;
}
inline uint8_t analagous_b(uint8_t c) {
	return (c + 22) % 12;
}

int main() {
	
	// the rtc has a slow start up time from power on, so just delay a bit here
	_delay_ms(500);
	
	DDRB |= _BV(1); // led strip output
	
	ws2811_t black = {.red = 0x00, .green = 0x00, .blue = 0x00 };
	ws2811_t grey = {.red = 0xcc, .green = 0xcc, .blue = 0xcc };
	ws2811_t white = {.red = 0xff, .green = 0xff, .blue = 0xff };

	ws2811_t red = {.red = 0xcc, .green = 0x00, .blue = 0x00 };			// 0
	ws2811_t green = {.red = 0x00, .green = 0xcc, .blue = 0x00 };		// 120
	ws2811_t blue = {.red = 0x00, .green = 0x00, .blue = 0xcc };		// 240

	ws2811_t yellow = {.red = 0xcc, .green = 0xcc, .blue = 0x00 };		// 60
	ws2811_t cyan = {.red = 0x00, .green = 0xcc, .blue = 0xcc };		// 180
	ws2811_t magenta = {.red = 0xcc, .green = 0x00, .blue = 0xcc };		// 300

	ws2811_t orange = {.red = 0xcc, .green = 0x66, .blue = 0x00 };		// 30
	ws2811_t chartreuse = {.red = 0x66, .green = 0xcc, .blue = 0x00 };	// 90
	ws2811_t spring = {.red = 0x00, .green = 0xcc, .blue = 0x66 };		// 150
	ws2811_t azure = {.red = 0x00, .green = 0x66, .blue = 0xcc };		// 210
	ws2811_t violet = {.red = 0x66, .green = 0x00, .blue = 0xcc };		// 270
	ws2811_t rose = {.red = 0xcc, .green = 0x00, .blue = 0x66 };		// 330

	ws2811_t palette[24] = { 
		red, orange, yellow, chartreuse, green, spring,
		cyan, azure, blue, violet, magenta, rose
	};
	
	struct ws2811_t colors[60];

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
	
	// Initialize RTC chip
	pcf8563_init();
	
	// Initialize serial
	serial_init_b(9600);

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
	struct pcf8563_t rtc;
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
	
	float hue = 0;	// current solid hue

	// hard coded location and time zone for now
	set_zone(-7 * ONE_HOUR);
	set_dst(usa_dst); // mountain
	set_position(183762, -410606); // calgary

	pcf8563_get(&rtc);
	sys.tm_year = 114;//rtc.year - 1900;
	sys.tm_mon = 5;//rtc.month - 1;				// rtc is 1 based; time lib is 0 based
	sys.tm_mday = 7;//rtc.mday;
	sys.tm_hour = 20;//rtc.hour;
	sys.tm_min = 23;//rtc.minute;
	systime = mktime(&sys);
	set_system_time(systime);
	
	uint8_t base_index = 0;
	
	struct ws2811_t base = black;
	struct ws2811_t markers = black;
	struct ws2811_t fill = black;
	struct ws2811_t other1 = black;
	struct ws2811_t other2 = black;
	
	while (1) {
		if (TCNT0 > 0) {
			system_tick();
			TCNT0 = 0;
			update = 1;
		}
		
		if (mode >= MODE_PLASMA && mode < MODE_YEAR && TCNT1 > 0x560) {
			TCNT1 = 0x00;
		//	update = 1;
		}
		
		// update display
		// * if the update flag is set and the remote isn't in the middle of receiving a message
		if (update == 1 && remote_state() == 0) {
			update = 0;
			
			PORTB |= _BV(PB0);
			
			base = palette[base_index];
			if (harmony == 0) {
				// complementary
				markers = grey;
				fill = base;
				other1 = palette[complementary(base_index)];
				other2 = base;
			} else if (harmony == 1) {
				// analogous
				markers = grey;
				fill = base;
				other1 = palette[analagous_a(base_index)];
				other2 = palette[analagous_b(base_index)];
			} else if (harmony == 2) {
				// split complementary
				markers = grey;
				fill = base;
				other1 = palette[analagous_a(complementary(base_index))];
				other2 = palette[analagous_b(complementary(base_index))];
			} else if (harmony == 3) {
				// triadic
				markers = grey;
				fill = base;
				other1 = palette[triad(base_index)];
				other2 = palette[triad(triad(base_index))];
			} else if (harmony == 4) {
				// tetradic
				markers = base;
				fill = palette[complementary(base_index)];
				other1 = palette[triad(base_index)];
				other2 = palette[complementary(triad(base_index))];
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
				colors[sys.tm_min] = other1;
				colors[sys.tm_sec] = other2;
			} else if (mode == MODE_MD) {
				// month, day of month
				// if (sys.tm_mon < 2) fill = azure; // jan, feb
				// else if (sys.tm_mon < 5) fill = spring; // mar, apr, may
				// else if (sys.tm_mon < 8) fill = rose; // jun, jul, aug
				// else if (sys.tm_mon < 11) fill = orange; // sep, oct, nov
				// else fill = azure; // dec
				
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
					uint8_t i = (sys.tm_mday - 1) * 2;
					colors[i] = other1;
					colors[i+1] = other1;
				} else {
					colors[58] = other1;
					colors[59] = other1;
					colors[0] = other1;
					colors[1] = other1;
				}
			} else if (mode == MODE_WD) {
				// week, day of week
				// markers
				colors[0] = markers;
				for (uint8_t i = 10; i < 60; i = i + 8) {
					colors[i] = markers;
				}
				// week fill
				uint8_t wday = sys.tm_wday * 8;
				for (uint8_t i = wday + 3; i < wday + 10; i++) {
					colors[i] = fill;
				}
				if (sys.tm_wday == 0) {
					colors[0] = fill;
					colors[1] = fill; 
				} else if (sys.tm_wday == 6) {
					colors[58] = fill;
					colors[59] = fill;
				}
				colors[week] = other1;
			} else if (mode == MODE_PCT) {
				// persoixante of day
				uint8_t ps = ((sys.tm_hour * 60) + sys.tm_min) / 24;
				uint8_t sr = ((rise.tm_hour * 60) + rise.tm_min) / 24;
				uint8_t ss = ((set.tm_hour * 60) + set.tm_min) / 24;
				for (uint8_t i = 0; i < ps; i ++) {
					colors[i] = fill;
				}
				colors[sr] = other1;
				colors[ss] = other2;
			} else if (mode == MODE_MOON) {
				// moon phase
				if (phase > 0) {
					// waxing
					for (int8_t i = 15 - phase; i < 15 + phase; i++) {
						if (i < 0) i = 60 + i;
						colors[i] = fill;
					}
				} else {
					// waning
					for (int8_t i = 45 - phase; i < 45 + phase; i++) {
						if (i > 60) i = i - 60;
						colors[i] = fill;
					}
				}
			} else if (mode == MODE_SPECTRUM) {
				for (uint8_t i = 0; i < 60; i++) {
					h2rgb(&colors[i], i * 6);
				}
			} else if (mode == MODE_PLASMA) {
				a(p1, p2, hues);
				a(p2, p3, hues);
				a(p3, p1, hues);

				for (int8_t i = 0; i < 60; i++) {
					h2rgb(&colors[i], hues[i]);
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
				hues[p1] += (r(4) - 1) * 3; // random 3 degree change in hue
				hues[p2] += (r(4) - 1) * 3;
				hues[p3] += (r(4) - 1) * 3;
				if (hues[p1] < 0) hues[p1] = 354;
				if (hues[p2] < 0) hues[p2] = 354;
				if (hues[p3] < 0) hues[p3] = 354;
				if (hues[p1] > 360) hues[p1] = 0;
				if (hues[p2] > 360) hues[p2] = 0;
				if (hues[p3] > 360) hues[p3] = 0;
			} else if (mode == MODE_SOLID) {
				hue += (r(4) - 1) * 3; // random 3 degree change in hue
				if (hue < 0) hue = 354;
				if (hue > 356) hue = 0;
				
				for (uint8_t i = 0; i < 60; i++) {
					h2rgb(&colors[i], hue);
				}
			} else if (mode == MODE_YEAR) {
				colors[sys.tm_year - 100] = yellow;
			} else if (mode == MODE_MONTH) {
				uint8_t mon = (sys.tm_mon % 12) * 5;
				for (uint8_t i = mon + 1; i < mon + 5; i++) {
					colors[i] = green;
				}
			} else if (mode == MODE_DAY) {
				if (sys.tm_mday < 31) {
					uint8_t i = (sys.tm_mday - 1) * 2;
					colors[i] = cyan;
					colors[i+1] = cyan;
				} else {
					colors[58] = cyan;
					colors[59] = cyan;
					colors[0] = cyan;
					colors[1] = cyan;
				}
			} else if (mode == MODE_HOUR) {
				for (uint8_t i = 0; i < 60; i = i + 5) {
					colors[i] = (sys.tm_hour > 11) ? chartreuse : violet;
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

			struct ws2811_t tx[60];
			for (int i = 0; i < 60; i++) tx[i] = colors[i]; //colors[(i + 30) % 60];
			ws2811_set(tx);
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
				if (mode < MODE_SOLID) mode++;
			} else if (command == REMOTE_MENU) {
				mode = MODE_YEAR;
				time(&systime);
				localtime_r(&systime, &sys);
				sys.tm_year -= 100;
				if (sys.tm_year < 0) sys.tm_year = 0;
			} else if (command == REMOTE_UP) {
				base_index++;
				base_index %= 12;
			} else if (command == REMOTE_DOWN) {
				base_index--;
				if (base_index > 12) base_index = 11;
			} else if (command == REMOTE_CENTER) {
				harmony++;
				harmony %= 5;
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
					// set the new system time
					sys.tm_year += 100;
					systime = mktime(&sys);
					set_system_time(systime);
					
					// store the new system time to the rtc
					rtc.year = sys.tm_year + 1900;	// years since 0
					rtc.month = sys.tm_mon + 1;		// rtc is 1 based; time lib is 0 based
					rtc.mday = sys.tm_mday;
					rtc.hour = sys.tm_hour;
					rtc.minute = sys.tm_min;
					rtc.second = sys.tm_sec;
					pcf8563_set(&rtc);
					mode = MODE_HMS;
				}
			} else if (command == REMOTE_MENU) {
				mode = MODE_HMS;
			}
		}
	}
}

EMPTY_INTERRUPT(TIMER0_OVF_vect)
