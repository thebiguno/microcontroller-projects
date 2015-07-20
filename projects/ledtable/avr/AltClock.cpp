#include "AltClock.h"
#include "lib/timer/timer.h"

using namespace digitalcave;

extern Hsv hsv;

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
	
	// modes are 16,8,12,6,20,10
	
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
			matrix.setColor(0,0,0);
			matrix.rectangle(0,0,11,11,DRAW_FILL);
			
			Hsv c = Hsv(hsv);
			matrix.setColor(Rgb(c));
			if (a & 0x01) matrix.rectangle(9,0,11,2,DRAW_FILL);
			if (a & 0x02) matrix.rectangle(6,0,8,2,DRAW_FILL);
			if (a & 0x04) matrix.rectangle(3,0,5,2,DRAW_FILL);
			if (a & 0x08) matrix.rectangle(0,0,2,2,DRAW_FILL);

			c.addHue(90);
			matrix.setColor(Rgb(c));
			if (b & 0x01) matrix.rectangle(9,3,11,5,DRAW_FILL);
			if (b & 0x02) matrix.rectangle(6,3,8,5,DRAW_FILL);
			if (b & 0x04) matrix.rectangle(3,3,5,5,DRAW_FILL);
			if (b & 0x08) matrix.rectangle(0,3,2,5,DRAW_FILL);
			
			c.addHue(90);
			matrix.setColor(Rgb(c));
			if (c & 0x01) matrix.rectangle(9,6,11,8,DRAW_FILL);
			if (c & 0x02) matrix.rectangle(6,6,8,8,DRAW_FILL);
			if (c & 0x04) matrix.rectangle(3,6,5,8,DRAW_FILL);
			if (c & 0x08) matrix.rectangle(0,6,2,8,DRAW_FILL);
			
			c.addHue(90);
			matrix.setColor(Rgb(c));
			if (d & 0x01) matrix.rectangle(9,9,11,11,DRAW_FILL);
			if (d & 0x02) matrix.rectangle(6,9,8,11,DRAW_FILL);
			if (d & 0x04) matrix.rectangle(3,9,5,11,DRAW_FILL);
			if (d & 0x08) matrix.rectangle(0,9,2,11,DRAW_FILL);
		}
		
		else if (running == 2) {
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

			matrix.setColor(0,0,0);
			matrix.rectangle(0,0,11,11,DRAW_FILL);
			
			// 6 horizontal binary bars with each bit as a 4x2 block
			Hsv c = Hsv(hsv);
			matrix.setColor(Rgb(c));
			if (a & 0x01) matrix.rectangle(8,0,11,1,DRAW_FILL);
			if (a & 0x02) matrix.rectangle(4,0,7,1,DRAW_FILL);
			if (a & 0x04) matrix.rectangle(0,0,3,1,DRAW_FILL);
			
			c.addHue(60);
			matrix.setColor(Rgb(c));
			if (b & 0x01) matrix.rectangle(8,2,11,3,DRAW_FILL);
			if (b & 0x02) matrix.drectangle(4,2,7,3,DRAW_FILL);
			if (b & 0x04) matrix.rectangle(0,2,3,3,DRAW_FILL);
			
			c.addHue(60);
			matrix.setColor(Rgb(c));
			if (c & 0x01) matrix.rectangle(8,4,11,5,DRAW_FILL);
			if (c & 0x02) matrix.rectangle(4,4,7,5,DRAW_FILL);
			if (c & 0x04) matrix.rectangle(0,4,3,5,DRAW_FILL);
			
			c.addHue(60);
			matrix.setColor(Rgb(c));
			if (d & 0x01) matrix.rectangle(8,6,11,7,DRAW_FILL);
			if (d & 0x02) matrix.rectangle(4,6,7,7,DRAW_FILL);
			if (d & 0x04) matrix.rectangle(0,6,3,7,DRAW_FILL;

			c.addHue(60);
			matrix.setColor(Rgb(c));
			if (e & 0x01) matrix.rectangle(8,8,11,9,DRAW_FILL);
			if (e & 0x02) matrix.rectangle(4,8,7,9,DRAW_FILL);
			if (e & 0x04) matrix.rectangle(0,8,3,9,DRAW_FILL);

			c.addHue(60);
			matrix.setColor(Rgb(c));
			if (f & 0x01) matrix.rectangle(8,10,11,11,DRAW_FILL);
			if (f & 0x02) matrix.rectangle(4,10,7,11,DRAW_FILL);
			if (f & 0x04) matrix.rectangle(0,10,3,11,DRAW_FILL);
		}
		
		else if (running == 3) {
			//milliseconds to dozenal (BBB.BB)
			a = ms / 7200000;			// 1/12 day = 2 h (shichen)
			ms -= 7200000 * (uint32_t) a;
			b = ms / 600000;			// 1/144 day = 10 m
			ms -= 600000 * (uint32_t) b;
			c = ms / 50000;				// 1/1728 day = 50 s
			ms -= 50000 * (uint32_t) c;
			d = ms / 4167;				// 1/20736 day ~= 4.167 s
			ms -= 4167 * (uint32_t) d;
			e = ms / 347;				// 1/248832 day ~= .347 s
			
			Hsv h = Hsv(hsv);
			matrix.setColor(Rgb(hsv));
			if (a == 0) matrix.line(5,0,6,0);
			else if (a == 1) matrix.line(7,0,8,0);
			else if (a == 2) matrix.line(11,3,11,4);
			else if (a == 3) matrix.line(11,5,11,6);
			else if (a == 4) matrix.line(11,7,11,8);
			else if (a == 5) matrix.line(7,11,8,11);
			else if (a == 6) matrix.line(5,11,6,11);
			else if (a == 7) matrix.line(3,11,4,11);
			else if (a == 8) matrix.line(0,7,0,8);
			else if (a == 9) matrix.line(0,5,0,6);
			else if (a == 10) matrix.line(0,2,0,4);
			else if (a == 11) matrix.line(3,0,4,0);

			h.addHue(90);
			matrix.setColor(Rgb(hsv));
			if (b == 0) matrix.line(5,1,6,1);
			else if (b == 1) matrix.line(7,1,8,1);
			else if (b == 2) matrix.line(10,3,10,4);
			else if (b == 3) matrix.line(10,5,10,6);
			else if (b == 4) matrix.line(10,7,10,8);
			else if (b == 5) matrix.line(7,10,8,10);
			else if (b == 6) matrix.line(5,10,6,10);
			else if (b == 7) matrix.line(3,10,4,10);
			else if (b == 8) matrix.line(1,7,1,8);
			else if (b == 9) matrix.line(1,5,1,6);
			else if (b == 10) matrix.line(1,2,1,4);
			else if (b == 11) matrix.line(3,1,4,1);
			
			h.addHue(90);
			matrix.setColor(Rgb(hsv));
			if (c == 0) matrix.line(5,2,6,2);
			else if (c == 1) matrix.line(7,2,8,2);
			else if (c == 2) matrix.line(9,3,9,4);
			else if (c == 3) matrix.line(9,5,9,6);
			else if (c == 4) matrix.line(9,7,9,8);
			else if (c == 5) matrix.line(7,9,8,9);
			else if (c == 6) matrix.line(5,9,6,9);
			else if (c == 7) matrix.line(3,9,4,9);
			else if (c == 8) matrix.line(2,7,2,8);
			else if (c == 9) matrix.line(2,5,2,6);
			else if (c == 10) matrix.line(2,2,2,4);
			else if (c == 11) matrix.line(3,2,4,2);
			
			h.addHue(90);
			matrix.setColor(Rgb(hsv));
			if (d == 0) matrix.line(5,3,6,3);
			else if (d == 1) matrix.line(7,3,8,3);
			else if (d == 2) matrix.line(8,3,8,4);
			else if (d == 3) matrix.line(8,5,8,6);
			else if (d == 4) matrix.line(8,7,8,8);
			else if (d == 5) matrix.line(7,8,8,8);
			else if (d == 6) matrix.line(5,8,6,8);
			else if (d == 7) matrix.line(3,8,4,8);
			else if (d == 8) matrix.line(3,7,3,8);
			else if (d == 9) matrix.line(3,5,3,6);
			else if (d == 10) matrix.line(3,2,3,4);
			else if (d == 11) matrix.line(3,3,4,3);

			h.setSaturation(0);
			matrix.setColor(Rgb(hsv));
			if (e & 0x01) {
				matrix.setPixel(5,5);
				matrix.setPixel(6,6);
			} else {
				matrix.setPixel(5,6);
				matrix.setPixel(6,5);
			}
		}
		
		if (running == 4) {
			//milliseconds to senary (555.555)
			a = ms / 14400000;		// 1/6 day = 4 h
			ms -= 14400000 * (uint32_t) a;
			b = Math.floor(ms / 2400000);		// 1/36 day = 40 m
			ms -= 2400000 * (uint32_t) b;
			ms *= 100;
			c = Math.floor(ms / 40000000);		// 1/216 day = 6 m 40 s
			ms -= 40000000 * (uint32_t) c;
			ms *= 100;
			d = Math.floor(ms / 666666666);		// 1/1296 day = 66.6 s
			ms -= 666666666 * (uint32_t) d;
			e = Math.floor(ms / 111111111);		// 1/7776 day = 11.1 s
			ms -= 111111111 * (uint32_t) e;
			ms *= 10;
			f = Math.floor(ms / 185185185);		// 1/46656 day = 1.85 s
			ms -= 185185185 * (uint32_t) f;
			g = Math.floor(ms / 30864197);		// 1/279936 day = 0.308 s
			
			Hsv h = Hsv(hsv);
			matrix.setColor(Rgb(hsv));
			if (a == 0) matrix.line(1,0,10,0);
			else if (a == 1) matrix.line(11,0,11,5);
			else if (a == 2) matrix.line(11,6,11,11);
			else if (a == 3) matrix.line(1,11,10,11);
			else if (a == 4) matrix.line(0,6,0,11);
			else if (a == 5) matrix.line(0,0,0,5);
			
			h.addHue(60);
			matrix.setColor(Rgb(hsv));
			if (b == 0) matrix.line(2,1,9,1);
			else if (b == 1) matrix.line(10,1,10,5);
			else if (b == 2) matrix.line(10,6,10,10);
			else if (b == 3) matrix.line(2,10,9,10);
			else if (b == 4) matrix.line(1,6,1,10);
			else if (b == 5) matrix.line(1,1,1,5);

			h.addHue(60);
			matrix.setColor(Rgb(hsv));
			if (c == 0) matrix.line(3,2,8,2);
			else if (c == 1) matrix.line(9,2,9,5);
			else if (c == 2) matrix.line(9,6,9,9);
			else if (c == 3) matrix.line(3,9,8,9);
			else if (c == 4) matrix.line(2,6,2,9);
			else if (c == 5) matrix.line(2,2,2,5);

			h.addHue(60);
			matrix.setColor(Rgb(hsv));
			if (d == 0) matrix.line(4,3,7,3);
			else if (d == 1) matrix.line(8,3,8,5);
			else if (d == 2) matrix.line(8,6,8,8);
			else if (d == 3) matrix.line(4,8,7,8);
			else if (d == 4) matrix.line(3,6,3,8);
			else if (d == 5) matrix.line(3,3,3,5);

			h.addHue(60);
			matrix.setColor(Rgb(hsv));
			if (e == 0) matrix.line(5,4,6,5);
			else if (e == 1) matrix.line(7,4,7,5);
			else if (e == 2) matrix.line(7,6,7,7);
			else if (e == 3) matrix.line(5,7,6,7);
			else if (e == 4) matrix.line(4,6,4,7);
			else if (e == 5) matrix.line(4,4,4,5);

			h.addHue(60);
			matrix.setColor(Rgb(hsv));
			if (f == 0) matrix.line(5,4,6,5);
			else if (f == 1) matrix.line(7,4,7,5);
			else if (f == 2) matrix.line(7,6,7,7);
			else if (f == 3) matrix.line(5,7,6,7);
			else if (f == 4) matrix.line(4,6,4,7);
			else if (f == 5) matrix.line(4,4,4,5);

			h.setSaturation(0);
			matrix.setColor(Rgb(hsv));
			if (g & 0x01) {
				matrix.setPixel(5,5);
				matrix.setPixel(6,6);
			} else {
				matrix.setPixel(5,6);
				matrix.setPixel(6,5);
			}
		}

		else if (running == 5) {
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
			Hsv h = Hsv(hsv);
			matrix.setColor(Rgb(hsv));
			if (x == 1) matrix.setPixel(1,1);
			if (x == 2) matrix.setPixel(2,1);
			if (x == 3) matrix.setPixel(3,1);
			if (x == 4) matrix.setPixel(4,1);
			if (a > 4) matrix.line(1,2,4,2);
			if (a > 9) matrix.line(1,3,4,3);
			if (a > 14) matrix.line(1,4,4,4);

			x = b % 5;
			if (x == 1) matrix.setPixel(7,1);
			if (x == 2) matrix.setPixel(8,1);
			if (x == 3) matrix.setPixel(9,1);
			if (x == 4) matrix.setPixel(10,1);
			if (b > 4) matrix.line(7,2,10,2);
			if (b > 9) matrix.line(7,3,10,3);
			if (b > 14) matrix.line(7,4,10,4);

			x = c % 5;
			if (x == 1) matrix.setPixel(1,7);
			if (x == 2) matrix.setPixel(2,7);
			if (x == 3) matrix.setPixel(3,7);
			if (x == 4) matrix.setPixel(4,7);
			if (c > 4) matrix.line(1,8,4,8);
			if (c > 9) matrix.line(1,9,4,9);
			if (c > 14) matrix.line(1,10,4,10);

			x = c % 5;
			if (x == 1) matrix.setPixel(7,7);
			if (x == 2) matrix.setPixel(8,7);
			if (x == 3) matrix.setPixel(9,7);
			if (x == 4) matrix.setPixel(10,7);
			if (d > 4) matrix.line(7,8,10,8);
			if (d > 9) matrix.line(7,9,10,9);
			if (d > 14) matrix.line(7,10,10,10);
		}
		
		else if (running == 6) {
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

			Hsv h = Hsv(hsv);
			matrix.setColor(Rgb(hsv));
			if (a == 0) matrix.line(5,0,6,0);
			else if (a == 1) matrix.line(7,0,8,0);
			else if (a == 2) matrix.line(11,4,11,5);
			else if (a == 3) matrix.line(11,6,11,7);
			else if (a == 4) matrix.line(7,11,8,11);
			else if (a == 5) matrix.line(5,11,6,11);
			else if (a == 6) matrix.line(3,11,4,11);
			else if (a == 7) matrix.line(0,6,0,7);
			else if (a == 8) matrix.line(0,4,0,5);
			else if (a == 9) matrix.line(3,0,4,0);
			
			h.addHue(90);
			matrix.setColor(Rgb(hsv));
			if (b == 0) matrix.line(5,1,6,1);
			else if (b == 1) matrix.line(7,1,8,1);
			else if (b == 2) matrix.line(10,4,10,5);
			else if (b == 3) matrix.line(10,6,10,7);
			else if (b == 4) matrix.line(7,10,8,10);
			else if (b == 5) matrix.line(5,10,6,10);
			else if (b == 6) matrix.line(3,10,4,10);
			else if (b == 7) matrix.line(1,6,1,7);
			else if (b == 8) matrix.line(1,4,1,5);
			else if (b == 9) matrix.line(3,1,4,1);

			h.addHue(90);
			matrix.setColor(Rgb(hsv));
			if (c == 0) matrix.line(5,2,6,2);
			else if (c == 1) matrix.line(7,2,8,2);
			else if (c == 2) matrix.line(9,4,9,5);
			else if (c == 3) matrix.line(9,6,9,7);
			else if (c == 4) matrix.line(7,9,8,9);
			else if (c == 5) matrix.line(5,9,6,9);
			else if (c == 6) matrix.line(3,9,4,9);
			else if (c == 7) matrix.line(1,6,1,7);
			else if (c == 8) matrix.line(1,4,1,5);
			else if (c == 9) matrix.line(3,1,4,1);

			h.addHue(90);
			matrix.setColor(Rgb(hsv));
			if (d == 0) matrix.line(5,3,6,3);
			else if (d == 1) matrix.line(7,3,8,3);
			else if (d == 2) matrix.line(8,4,8,5);
			else if (d == 3) matrix.line(8,6,8,7);
			else if (d == 4) matrix.line(7,8,8,8);
			else if (d == 5) matrix.line(5,8,6,8);
			else if (d == 6) matrix.line(3,8,4,8);
			else if (d == 7) matrix.line(3,6,3,7);
			else if (d == 8) matrix.line(3,4,3,5);
			else if (d == 9) matrix.line(3,3,4,3);

			h.setSaturation(0);
			matrix.setColor(Rgb(hsv));
			if (g & 0x01) {
				matrix.setPixel(5,5);
				matrix.setPixel(6,6);
			} else {
				matrix.setPixel(5,6);
				matrix.setPixel(6,5);
			}
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