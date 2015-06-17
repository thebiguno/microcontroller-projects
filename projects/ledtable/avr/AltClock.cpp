#include "AltClock.h"
#include "lib/timer/timer.h"

AltClock::run() {
	uint8_t running = 0;

	uint8_t a;
	uint8_t b;
	uint8_t c;
	uint8_t d;
	uint8_t e;
	uint8_t f;
	
	mcp79410_time_t time;
	mcp79410_get(time);

	uint32_t millis = time.hour * 3600000 + time.minute * 60000 + time.seconds * 1000;
	timer_init();
	
	while (running > 0) {
		
		uint32_t ms = millis + timer_millis();
		
		if (ms > 86400000) {
			millis = 0;
			timer_init();
		}
		
		uint8_t values[6];
		
		if (running == 1) {
			// This method was first proposed in the 1850s by John W. Nystrom
			// milliseconds to hexadecimal (FF.FF)
			a = ms / 5400000;			// 1/16 day (hex hour) = 1 h 30 m
			ms -= 5400000 * (uint32_t) a;
			b = ms / 337500;			// 1/256 day (hex maxime) = 5 m 37.5 s
			ms -= 337500 * (uint32_t) b;
			ms *= 100;					// bump up the precision
			c = ms / 2109375;			// 1/4096 day (hex minute) ~= 21 seconds
			ms -= 2109375 * (uint32_t) c;
			ms *= 100;					// bump up the precision again
			d = ms / 13183593;			// 1/65536 day (hex second) ~= 1.32 seconds
			
			// 4 horizontal binary bars with each bit as a 3x3 block
			draw_rectangle(0,0,11,11,DRAW_FILL,color.black,DRAW_REPLACE);
			if (a & 0x01) draw_rectangle(9,0,11,2,DRAW_FILL,color.red,DRAW_REPLACE);
			if (a & 0x02) draw_rectangle(6,0,8,2,DRAW_FILL,color.red,DRAW_REPLACE);
			if (a & 0x04) draw_rectangle(3,0,5,2,DRAW_FILL,color.red,DRAW_REPLACE);
			if (a & 0x08) draw_rectangle(0,0,2,2,DRAW_FILL,color.red,DRAW_REPLACE);
			
			if (b & 0x01) draw_rectangle(9,3,11,5,DRAW_FILL,color.chartreuse,DRAW_REPLACE);
			if (b & 0x02) draw_rectangle(6,3,8,5,DRAW_FILL,color.chartreuse,DRAW_REPLACE);
			if (b & 0x04) draw_rectangle(3,3,5,5,DRAW_FILL,color.chartreuse,DRAW_REPLACE);
			if (b & 0x08) draw_rectangle(0,3,2,5,DRAW_FILL,color.chartreuse,DRAW_REPLACE);
			
			if (c & 0x01) draw_rectangle(9,6,11,8,DRAW_FILL,color.cyan,DRAW_REPLACE);
			if (c & 0x02) draw_rectangle(6,6,8,8,DRAW_FILL,color.cyan,DRAW_REPLACE);
			if (c & 0x04) draw_rectangle(3,6,5,8,DRAW_FILL,color.cyan,DRAW_REPLACE);
			if (c & 0x08) draw_rectangle(0,6,2,8,DRAW_FILL,color.cyan,DRAW_REPLACE);
			
			if (d & 0x01) draw_rectangle(9,9,11,11,DRAW_FILL,color.violet,DRAW_REPLACE);
			if (d & 0x02) draw_rectangle(6,9,8,11,DRAW_FILL,color.violet,DRAW_REPLACE);
			if (d & 0x04) draw_rectangle(3,9,5,11,DRAW_FILL,color.violet,DRAW_REPLACE);
			if (d & 0x08) draw_rectangle(0,9,2,11,DRAW_FILL,color.violet,DRAW_REPLACE);
		}
		
		if (running == 2) {
			// milliseconds to octal (777.777)
			a = ms / 10800000;			// 1/8 day = 3 h
			ms -= 10800000 * (uint32_t) a ;
			b = ms / 1350000			// 1/64 day = 22 m 30 s
			ms -= 1350000 * (uint32_t) b;
			c = ms / 168750;			// 1/512 day ~= 2 m 49 s
			ms -= 168750 * (uint32_t) c;
			ms *= 100;					// bump up the precision
			d = ms / 2109375;			// 1/4096 day ~= 21 s
			ms -= 2109375 * (uint32_t) d;
			ms *= 100;					// bump up the precision
			e = ms / 26367187;			// 1/32768 day ~= 2.63 s
			ms -= 26367187 * (uint32_t) e;
			ms *= 100;
			f = ms / 329589843;			// 1/262144 day ~= .329 s
			
			// 6 horizontal binary bars with each bit as a 4x2 block
			draw_rectangle(0,0,11,11,DRAW_FILL,color.black,DRAW_REPLACE);
			if (a & 0x01) draw_rectangle(8,0,11,1,DRAW_FILL,color.red,DRAW_REPLACE);
			if (a & 0x02) draw_rectangle(4,0,7,1,DRAW_FILL,color.red,DRAW_REPLACE);
			if (a & 0x04) draw_rectangle(0,0,3,1,DRAW_FILL,color.red,DRAW_REPLACE);
			
			if (b & 0x01) draw_rectangle(8,2,11,3,DRAW_FILL,color.yellow,DRAW_REPLACE);
			if (b & 0x02) draw_rectangle(4,2,7,3,DRAW_FILL,color.yellow,DRAW_REPLACE);
			if (b & 0x04) draw_rectangle(0,2,3,3,DRAW_FILL,color.yellow,DRAW_REPLACE);
			
			if (c & 0x01) draw_rectangle(8,4,11,5,DRAW_FILL,color.green,DRAW_REPLACE);
			if (c & 0x02) draw_rectangle(4,4,7,5,DRAW_FILL,color.green,DRAW_REPLACE);
			if (c & 0x04) draw_rectangle(0,4,3,5,DRAW_FILL,color.green,DRAW_REPLACE);
			
			if (d & 0x01) draw_rectangle(8,6,11,7,DRAW_FILL,color.cyan,DRAW_REPLACE);
			if (d & 0x02) draw_rectangle(4,6,7,7,DRAW_FILL,color.cyan,DRAW_REPLACE);
			if (d & 0x04) draw_rectangle(0,6,3,7,DRAW_FILL,color.cyan,DRAW_REPLACE);

			if (e & 0x01) draw_rectangle(8,8,11,9,DRAW_FILL,color.blue,DRAW_REPLACE);
			if (e & 0x02) draw_rectangle(4,8,7,9,DRAW_FILL,color.blue,DRAW_REPLACE);
			if (e & 0x04) draw_rectangle(0,8,3,9,DRAW_FILL,color.blue,DRAW_REPLACE);

			if (f & 0x01) draw_rectangle(8,10,11,11,DRAW_FILL,color.magenta,DRAW_REPLACE);
			if (f & 0x02) draw_rectangle(4,10,7,11,DRAW_FILL,color.magenta,DRAW_REPLACE);
			if (f & 0x04) draw_rectangle(0,10,3,11,DRAW_FILL,color.magenta,DRAW_REPLACE);
		}
		
		if (running == 3) {
			//milliseconds to decimal (BBB.BB)
			a = ms / 7200000;			// 1/12 day = 2 h (shichen)
			ms -= 7200000 * (uint32_t) a;
			b = ms / 600000;			// 1/144 day = 10 m
			ms -= 600000 * (uint32_t) b;
			c = ms / 50000;				// 1/1728 day = 50 s
			ms -= 50000 * (uint32_t) c;
			d = ms / 4167;				// 1/20736 day ~= 4.167 s
			ms -= 4167 * (uint32_t) d;
			e = ms / 347;				// 1/248832 day ~= .347 s
			
			draw_rectangle(0,0,11,11,DRAW_FILL,color.black,DRAW_REPLACE);
			draw_line(a,0,a,2,color.red,DRAW_REPLACE);
			draw_line(b,3,b,5,color.chartreuse,DRAW_REPLACE);
			draw_line(c,6,c,8,color.cyan,DRAW_REPLACE);
			draw_line(d,9,d,11,color.violet,DRAW_REPLACE);
			if (e & 0x01) set_pixel(11,11,color.white);
		}

		if (running == 4) {
			//this method was introduced during the French Revolution in 1793
			//milliseconds to decimal (999.99)
			a = ms / 8640000;			// 1/10 day (deciday) = 2 h 24 m (shi)
			ms -= 8640000 * (uint32_t) a;
			b = ms / 864000;			// 1/100 day (centiday) = 14 m 24 s
			ms -= 864000 * (uint32_t) b;
			c = ms / 86400;				// 1/1000 day (milliday; beat) = 1 m 26.4 s (ke)
			ms -= 86400 * (uint32_t) c;
			d = ms / 8640;				// 1/10000 day = 8.64 s (decibeat)
			ms -= 8640 * (uint32_t) d
			e = ms / 864;				// 1/100000 day (centibeat) / .864 s (fen)

			draw_rectangle(0,0,11,11,DRAW_FILL,color.black,DRAW_REPLACE);
			draw_line(a,0,a,2,color.red,DRAW_REPLACE);
			draw_line(b,3,b,5,color.chartreuse,DRAW_REPLACE);
			draw_line(c,6,c,8,color.cyan,DRAW_REPLACE);
			draw_line(d,9,d,11,color.violet,DRAW_REPLACE);
			if (e & 0x01) set_pixel(11,11,color.white);
		}
		
		if (running == 5) {
			//milliseconds to vigesimal (19.19.19.19)
			a = ms / 4320000;			// 1/20 day = 1 h 12 m
			ms -= 4320000 * (uint32_t) a;
			b = ms / 216000;			// 1/400 day = 3 m 36 s
			ms -= 216000 * (uint32_t) b;
			c = ms / 10800;				// 1/8000 day = 10.8 s
			ms -= 10800 * (uint32_t) c;
			d = ms / 540;				// 1/160000 day = .54 s
			
			// 4 mayan blocks
			uint8_t x = a % 5;
			if (x == 1) set_pixel(1,1,color.red);
			if (x == 2) set_pixel(2,1,color.red);
			if (x == 3) set_pixel(3,1,color.red);
			if (x == 4) set_pixel(4,1,color.red);
			if (a > 4) draw_line(1,2,4,2,color.red,DRAW_REPLACE);
			if (a > 9) draw_line(1,3,4,3,color.red,DRAW_REPLACE);
			if (a > 14) draw_line(1,4,4,4,color.red,DRAW_REPLACE);

			x = b % 5;
			if (x == 1) set_pixel(7,1,color.chartreuse);
			if (x == 2) set_pixel(8,1,color.chartreuse);
			if (x == 3) set_pixel(9,1,color.chartreuse);
			if (x == 4) set_pixel(10,1,color.chartreuse);
			if (b > 4) draw_line(7,2,10,2,color.chartreuse,DRAW_REPLACE);
			if (b > 9) draw_line(7,3,10,3,color.chartreuse,DRAW_REPLACE);
			if (b > 14) draw_line(7,4,10,4,color.chartreuse,DRAW_REPLACE);

			x = c % 5;
			if (x == 1) set_pixel(1,7,color.cyan);
			if (x == 2) set_pixel(2,7,color.cyan);
			if (x == 3) set_pixel(3,7,color.cyan);
			if (x == 4) set_pixel(4,7,color.cyan);
			if (c > 4) draw_line(1,8,4,8,color.cyan,DRAW_REPLACE);
			if (c > 9) draw_line(1,9,4,9,color.cyan,DRAW_REPLACE);
			if (c > 14) draw_line(1,10,4,10,color.cyan,DRAW_REPLACE);

			x = c % 5;
			if (x == 1) set_pixel(7,7,color.violet);
			if (x == 2) set_pixel(8,7,color.violet);
			if (x == 3) set_pixel(9,7,color.violet);
			if (x == 4) set_pixel(10,7,color.violet);
			if (d > 4) draw_line(7,8,10,8,color.violet,DRAW_REPLACE);
			if (d > 9) draw_line(7,9,10,9,color.violet,DRAW_REPLACE);
			if (d > 14) draw_line(7,10,10,10,color.violet,DRAW_REPLACE);
		}
	
		if (running == 6) {
			//milliseconds to d'ni (24:24:24)
			a = ms / 3456000;			// 1/25 day = 57 m 36 s
			ms -= 3456000 * (uint32_t) a;
			b = ms / 138240;			// 1/625 day = 2 m 18.24 s
			ms -= 138240 * (uint32_t) b;
			ms *= 10;					// bump up the precision
			c = ms / 55296;				// 1/15625 day ~= 5.5 s
			ms -= 55296 * (uint32_t) c;
			d = ms / 2212;				// 1/390625 day ~= .22 s
		}
		
		void psx_read_gamepad();
		buttons = psx_buttons();
		if (buttons & PSB_PAD_UP) running++;
		else if (buttons & PSB_PAD_DOWN) running--;
		else if (buttons & PSB_PAD_LEFT) running--;
		else if (buttons & PSB_PAD_RIGHT) running++;
		
		if (running > 6) running == 1;
		if (running < 1) running == 6;
		
		if (buttons & PSB_TRIANGLE) running = 0;
	}
}