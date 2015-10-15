#include "Clock.h"
#include "ClockSet.h"
#include <Buttons.h>
#include <Hsv.h>
#include <Rgb.h>
#include "Matrix.h"
#include "lib/rtc/mcp79410/mcp79410.h"
#include <util/delay.h>
#include <stdlib.h>

using namespace digitalcave;

extern Hsv hsv;
extern Matrix matrix;
extern Buttons buttons;
extern uint8_t sample;

Clock::Clock() {
}

Clock::~Clock() {
}

void Clock::run() {
	mcp79410_time_t time;
	uint8_t running = 1;

	uint8_t second = 0;
	uint8_t tick = 0;
	uint32_t ms;
	
	uint8_t released;
	uint8_t held;

	uint8_t field[4];
	char c[4];

	while (running) {
		mcp79410_get(&time);
		
		tick++; // this code should take about 100 ms
		
		if (time.second != second) {
			// change the hue every second
			second = time.second;
			hsv.addHue(1);
			
			tick = 0;
		}
		
		if (running == 1) {
			field[0] = time.hour / 12;
			field[1] = time.hour - (12 * field[0]);
			field[2] = time.minute / 10;
			field[3] = time.minute - (10 * field[2]);
		}
		else if (running == 2) {
			ms = (time.hour * 3600000) + (time.minute * 60000) + (time.second * 1000) + (tick * 100); 

			// This method was first proposed in the 1850s by John W. Nystrom
			// milliseconds to hexadecimal (FF.FF)
			field[0] = ms / 5400000;			// 1/16 day (hex hour) = 1 h 30 m
			ms -= 5400000 * (uint32_t) field[0];
			field[1] = ms / 337500;			// 1/256 day (hex maxime) = 5 m 37.5 s
			ms -= 337500 * (uint32_t) field[1];
			ms *= 100;					// bump up the precision
			field[2] = ms / 2109375;			// 1/4096 day (hex minute) ~= 21 seconds
			ms -= 2109375 * (uint32_t) field[2];
			ms *= 100;					// bump up the precision again
			field[3] = ms / 13183593;			// 1/65536 day (hex second) ~= 1.32 seconds
		}
		else if (running == 3) {
			ms = (time.hour * 3600000) + (time.minute * 60000) + (time.second * 1000) + (tick * 12); 

			// milliseconds to octal (777.777)
			field[0] = ms / 10800000;			// 1/8 day = 3 h
			ms -= 10800000 * (uint32_t) field[0];
			field[1] = ms / 1350000;			// 1/64 day = 22 m 30 s
			ms -= 1350000 * (uint32_t) field[1];
			field[2] = ms / 168750;			// 1/512 day ~= 2 m 49 s
			ms -= 168750 * (uint32_t) field[2];
			ms *= 100;					// bump up the precision
			field[3] = ms / 2109375;			// 1/4096 day ~= 21 s
		}
		else if (running == 4) {
			ms = (time.hour * 3600000) + (time.minute * 60000) + (time.second * 1000) + (tick * 12); 

			//milliseconds to dozenal (BBB.BB)
			field[0] = ms / 7200000;			// 1/12 day = 2 h (shichen)
			ms -= 7200000 * (uint32_t) field[0];
			field[1] = ms / 600000;			// 1/144 day = 10 m
			ms -= 600000 * (uint32_t) field[1];
			field[2] = ms / 50000;				// 1/1728 day = 50 s
			ms -= 50000 * (uint32_t) field[2];
			field[3] = ms / 4167;				// 1/20736 day ~= 4.167 s
		}
		if (running == 5) {
			ms = (time.hour * 3600000) + (time.minute * 60000) + (time.second * 1000) + (tick * 12); 

			//milliseconds to senary (555.555)
			field[0] = ms / 14400000;		// 1/6 day = 4 h
			ms -= 14400000 * (uint32_t) field[0];
			field[1] = floor(ms / 2400000);		// 1/36 day = 40 m
			ms -= 2400000 * (uint32_t) field[1];
			ms *= 100;
			field[2] = floor(ms / 40000000);		// 1/216 day = 6 m 40 s
			ms -= 40000000 * (uint32_t) field[2];
			ms *= 100;
			field[3] = floor(ms / 666666666);		// 1/1296 day = 66.6 s
		}
		else if (running == 6) {
			ms = (time.hour * 3600000) + (time.minute * 60000) + (time.second * 1000) + (tick * 12); 

			//milliseconds to vigesimal (19.19.19.19)
			field[0] = ms / 4320000;			// 1/20 day = 1 h 12 m
			ms -= 4320000 * (uint32_t) field[0];
			field[1] = ms / 216000;			// 1/400 day = 3 m 36 s
			ms -= 216000 * (uint32_t) field[1];
			field[2] = ms / 10800;				// 1/8000 day = 10.8 s
			ms -= 10800 * (uint32_t) field[2];
			field[3] = ms / 540;				// 1/160000 day = .54 s
		}
		else if (running == 7) {
			ms = (time.hour * 3600000) + (time.minute * 60000) + (time.second * 1000) + (tick * 12); 

			//this method was introduced during the French Revolution in 1793
			//milliseconds to decimal (999.99)
			field[0] = ms / 8640000;			// 1/10 day (deciday) = 2 h 24 m (shi)
			ms -= 8640000 * (uint32_t) field[0];
			field[1] = ms / 864000;			// 1/100 day (centiday) = 14 m 24 s
			ms -= 864000 * (uint32_t) field[1];
			field[2] = ms / 86400;				// 1/1000 day (milliday; beat) = 1 m 26.4 s (ke)
			ms -= 86400 * (uint32_t) field[2];
			field[3] = ms / 8640;				// 1/10000 day = 8.64 s (decibeat)
		}
		
		for (uint8_t i = 0; i < 4; i++) {
			uint8_t f = field[i];
			switch (f) {
				case 0: c[i] = '0'; break;
				case 1: c[i] = '1'; break;
				case 2: c[i] = '2'; break;
				case 3: c[i] = '3'; break;
				case 4: c[i] = '4'; break;
				case 5: c[i] = '5'; break;
				case 6: c[i] = '6'; break;
				case 7: c[i] = '7'; break;
				case 8: c[i] = '8'; break;
				case 9: c[i] = '9'; break;
				case 10: c[i] = 'A'; break;
				case 11: c[i] = 'B'; break;
				case 12: c[i] = 'C'; break;
				case 13: c[i] = 'D'; break;
				case 14: c[i] = 'E'; break;
				case 15: c[i] = 'F'; break;
				case 16: c[i] = 'G'; break;
				case 17: c[i] = 'H'; break;
				case 18: c[i] = 'I'; break;
				case 19: c[i] = 'J'; break;
			}
		}
		
		// draw
		matrix.setColor(0,0,0);
		matrix.rectangle(0,0,11,11,DRAW_FILLED);
		
		matrix.setColor(Rgb(hsv));
		matrix.character(0,0, c[0], 0);
		matrix.character(6,0, c[1], 0);
		matrix.character(0,6, c[2], 0);
		matrix.character(6,6, c[3], 0);

		matrix.flush();
		
		// sample buttons
		if (sample) {
			buttons.sample();
			released = buttons.released();
			held = buttons.held();

			sample = 0;
		}

		// take action
		if (held && 0x01) {
			// exit
			running = 0;
		}
		else if (released && 0x01) {
			// change clock (trad, hexidecimal, octal, dozenal, senary, vigesimal, decimal)
			running++;
			running %= 8;
			if (running == 0) running = 1;
			
			matrix.setColor(0,0,0);
			matrix.rectangle(0,0,11,11,DRAW_FILLED);
			if (running == 1) {
				matrix.text(0, 0, "HH", 0);
				matrix.text(0, 0, "MM", 0);
			} else if (running == 2) {
				matrix.text(0, 0, "FF", 0);
				matrix.text(0, 0, "FF", 0);
			} else if (running == 3) {
				matrix.text(0, 0, "77", 0);
				matrix.text(0, 0, "77", 0);
			} else if (running == 4) {
				matrix.text(0, 0, "BB", 0);
				matrix.text(0, 0, "BB", 0);
			} else if (running == 5) {
				matrix.text(0, 0, "55", 0);
				matrix.text(0, 0, "55", 0);
			} else if (running == 6) {
				matrix.text(0, 0, "JJ", 0);
				matrix.text(0, 0, "JJ", 0);
			} else if (running == 7) {
				matrix.text(0, 0, "99", 0);
				matrix.text(0, 0, "99", 0);
			}
			matrix.flush();
			_delay_ms(1000);
		}
		else if (held && 0x02) {
			// set date/time
			ClockSet cs = ClockSet();
			cs.run();
		}
		else if (released && 0x02) {
			// noop
		}
		
		_delay_ms(99);
	}
}
