#include "Clock.h"
#include "lib/Psx/Psx.h"
#include "lib/Draw/Hsv.h"
#include "lib/Draw/Rgb.h"
#include "Matrix.h"
#include "lib/rtc/mcp79410/mcp79410.h"
#include <util/delay.h>
#include <stdlib.h>

using namespace digitalcave;

extern Hsv hsv;
extern Matrix matrix;
extern Psx psx;

Clock::Clock() {
}

Clock::~Clock() {
}

void Clock::run() {
	mcp79410_time_t time;
	uint16_t buttons;
	uint16_t changed;
	uint8_t running = 1;
	uint8_t set = 0;

	while (running) {
		if (!set) mcp79410_get(&time);
		
		matrix.setColor(0,0,0);
		matrix.rectangle(0,0,11,11,DRAW_FILLED);
		
		if (running == 1) {
			// hour and minute hands drawn as lines
			// 5 minute accuracy
			uint8_t h = time.hour % 12;
			uint8_t m = time.minute / 5;
			
			matrix.setColor(Rgb(Hsv(0,0,hsv.getValue())));
			matrix.setPixel(5, 0);
			matrix.setPixel(0, 5);
			matrix.setPixel(10, 5);
			matrix.setPixel(5, 10);
			
			Hsv c = Hsv(hsv);
			matrix.setColor(Rgb(c));
			
			if (m == 0) matrix.line(5, 5, 5, 0);
			else if (m == 1) matrix.line(5, 5, 7, 0);
			else if (m == 2) matrix.line(5, 5, 10, 3);
			else if (m == 3) matrix.line(5, 5, 10, 5);
			else if (m == 4) matrix.line(5, 5, 10, 7);
			else if (m == 5) matrix.line(5, 5, 7, 10);
			else if (m == 6) matrix.line(5, 5, 5, 10);
			else if (m == 7) matrix.line(5, 5, 3, 10);
			else if (m == 8) matrix.line(5, 5, 0, 7);
			else if (m == 9) matrix.line(5, 5, 0, 5);
			else if (m == 10) matrix.line(5, 5, 0, 3);
			else if (m == 11) matrix.line(5, 5, 3, 0);
			
			c.addHue(180);
			matrix.setColor(Rgb(c));
			
			if (h == 0) matrix.line(5, 5, 5, 2);
			else if (h == 1) matrix.line(5, 5, 6, 2);
			else if (h == 2) matrix.line(5, 5, 8, 4);
			else if (h == 3) matrix.line(5, 5, 8, 5);
			else if (h == 4) matrix.line(5, 5, 8, 6);
			else if (h == 5) matrix.line(5, 5, 6, 8);
			else if (h == 6) matrix.line(5, 5, 5, 8);
			else if (h == 7) matrix.line(5, 5, 4, 8);
			else if (h == 8) matrix.line(5, 5, 2, 6);
			else if (h == 9) matrix.line(5, 5, 2, 5);
			else if (h == 10) matrix.line(5, 5, 2, 4);
			else if (h == 11) matrix.line(5, 5, 4, 2);
		}
		else if (running == 2) {
			// hour and minute hands drawn as blocks
			// 5 minute accuracy
			uint8_t h = time.hour % 12;
			uint8_t m = time.minute / 5;
			
			matrix.setColor(Rgb(Hsv(0,0,hsv.getValue())));
			matrix.rectangle(2, 2, 9, 9, DRAW_UNFILLED);

			Hsv c = Hsv(hsv);
			matrix.setColor(Rgb(c));
			
			if (m == 0) matrix.rectangle(5, 0, 6, 1,DRAW_FILLED);
			else if (m == 1) matrix.rectangle(7, 0, 8, 1,DRAW_FILLED);
			else if (m == 2) matrix.rectangle(10, 3, 11, 4,DRAW_FILLED);
			else if (m == 3) matrix.rectangle(10, 5, 11, 6,DRAW_FILLED);
			else if (m == 4) matrix.rectangle(10, 7, 11, 8,DRAW_FILLED);
			else if (m == 5) matrix.rectangle(7, 10, 8, 11,DRAW_FILLED);
			else if (m == 6) matrix.rectangle(5, 10, 6, 11,DRAW_FILLED);
			else if (m == 7) matrix.rectangle(3, 10, 4, 11,DRAW_FILLED);
			else if (m == 8) matrix.rectangle(0, 7, 1, 8,DRAW_FILLED);
			else if (m == 9) matrix.rectangle(0, 5, 1, 6,DRAW_FILLED);
			else if (m == 10) matrix.rectangle(0, 3, 1, 4,DRAW_FILLED);
			else if (m == 11) matrix.rectangle(3, 0, 4, 1,DRAW_FILLED);
			
			c.addHue(180);
			matrix.setColor(Rgb(c));
			
			if (h == 0) matrix.rectangle(5, 3, 6, 4,DRAW_FILLED);
			else if (h == 1) matrix.rectangle(6, 3, 7, 4,DRAW_FILLED);
			else if (h == 2) matrix.rectangle(7, 4, 8, 5,DRAW_FILLED);
			else if (h == 3) matrix.rectangle(7, 5, 8, 6,DRAW_FILLED);
			else if (h == 4) matrix.rectangle(7, 7, 8, 8,DRAW_FILLED);
			else if (h == 5) matrix.rectangle(6, 7, 7, 8,DRAW_FILLED);
			else if (h == 6) matrix.rectangle(5, 7, 6, 8,DRAW_FILLED);
			else if (h == 7) matrix.rectangle(4, 7, 5, 8,DRAW_FILLED);
			else if (h == 8) matrix.rectangle(3, 6, 4, 7,DRAW_FILLED);
			else if (h == 9) matrix.rectangle(3, 5, 4, 6,DRAW_FILLED);
			else if (h == 10) matrix.rectangle(3, 4, 4, 5,DRAW_FILLED);
			else if (h == 11) matrix.rectangle(4, 3, 5, 4,DRAW_FILLED);
		}
		else if (running == 3) {
			// BCD
			// 1 second accuracy
			uint8_t one = time.hour;
			uint8_t ten = 0;
			while (one > 10) { 
				ten++;
				one -= 10;
			}
			Hsv c = Hsv(hsv);
			matrix.setColor(Rgb(c));
			if (ten & 0x01) matrix.rectangle(9,0,11,1,DRAW_FILLED);
			if (ten & 0x02) matrix.rectangle(6,0,8,1,DRAW_FILLED);
			
			if (one & 0x01) matrix.rectangle(9,2,11,3,DRAW_FILLED);
			if (one & 0x02) matrix.rectangle(6,2,8,3,DRAW_FILLED);
			if (one & 0x04) matrix.rectangle(3,2,5,3,DRAW_FILLED);
			if (one & 0x08) matrix.rectangle(0,2,2,3,DRAW_FILLED);

			c.addHue(120);
			matrix.setColor(Rgb(c));
			one = time.minute;
			ten = 0;
			while (one > 10) { 
				ten++;
				one -= 10;
			}
			if (ten & 0x01) matrix.rectangle(9,4,11,5,DRAW_FILLED);
			if (ten & 0x02) matrix.rectangle(6,4,8,5,DRAW_FILLED);
			if (ten & 0x04) matrix.rectangle(3,4,5,5,DRAW_FILLED);
			
			if (one & 0x01) matrix.rectangle(9,6,11,7,DRAW_FILLED);
			if (one & 0x02) matrix.rectangle(6,6,8,7,DRAW_FILLED);
			if (one & 0x04) matrix.rectangle(3,6,5,7,DRAW_FILLED);
			if (one & 0x08) matrix.rectangle(0,6,2,7,DRAW_FILLED);

			c.addHue(120);
			matrix.setColor(Rgb(c));
			one = time.second;
			ten = 0;
			while (one > 10) { 
				ten++;
				one -= 10;
			}
			if (ten & 0x01) matrix.rectangle(9,8,11,9,DRAW_FILLED);
			if (ten & 0x02) matrix.rectangle(6,8,8,9,DRAW_FILLED);
			if (ten & 0x04) matrix.rectangle(3,8,5,9,DRAW_FILLED);
			
			if (one & 0x01) matrix.rectangle(9,10,11,11,DRAW_FILLED);
			if (one & 0x02) matrix.rectangle(6,10,8,11,DRAW_FILLED);
			if (one & 0x04) matrix.rectangle(3,10,5,11,DRAW_FILLED);
			if (one & 0x08) matrix.rectangle(0,10,2,11,DRAW_FILLED);
		}
		else if (running == 4) {
			// one pixel for every hour, minute, second
			uint8_t h = time.hour;
			uint8_t m = time.minute;
			uint8_t s = time.second;
			
			uint8_t y = h / 12;
			uint8_t x = h % 12;
			
			Hsv c = Hsv(hsv);
			matrix.setColor(Rgb(c));
			matrix.setPixel(x,y);
			
			y = (m / 12) + 2;
			x = m % 12;
			
			c.addHue(120);
			matrix.setColor(Rgb(c));
			matrix.setPixel(x,y);
			
			y = (s / 12) + 7;
			x = s % 12;
			
			c.addHue(120);
			matrix.setColor(Rgb(c));
			matrix.setPixel(x,y);
		}

		matrix.flush();
		
		psx.poll();
		buttons = psx.buttons();
		changed = psx.changed();
		if (set) {
			if (buttons & PSB_PAD_UP && changed & PSB_PAD_UP) {
				time.hour++;
				time.hour %= 24;
			}
			else if (buttons & PSB_PAD_DOWN && changed & PSB_PAD_DOWN) {
				if (time.hour > 0) time.hour--;
				time.hour %= 24;
			}
			else if (buttons & PSB_PAD_LEFT && changed & PSB_PAD_LEFT) {
				time.minute--;
				time.minute %= 60;
			}
			else if (buttons & PSB_PAD_RIGHT && changed & PSB_PAD_RIGHT) {
				if (time.minute > 0) time.minute++;
				time.minute %= 60;
			}
			else if (buttons & PSB_SELECT && changed & PSB_SELECT) {
				time.second = 0;
				mcp79410_set(&time);
				set = 0;
				running = 1;
			}
		}
		else {
			if (buttons & PSB_PAD_LEFT && changed & PSB_PAD_LEFT) {
				running--;
				if (running == 0) running = 4;
			}
			else if (buttons & PSB_PAD_RIGHT && changed & PSB_PAD_RIGHT) {
				running++;
				running %= 5;
				if (running == 0) running = 1;
			}
			else if (buttons & PSB_SELECT && changed & PSB_SELECT) {
				running = 4;
				set = 1;
			}

			if (buttons & PSB_START & changed & PSB_START) running = 0;
		}
		
		_delay_ms(100);
	}
}
