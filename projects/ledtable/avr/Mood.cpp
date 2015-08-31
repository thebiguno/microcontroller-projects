#include "Mood.h"
#include "Matrix.h"
#include "lib/Draw/Hsv.h"
#include "lib/Draw/Rgb.h"
#include "lib/Psx/Psx.h"
#include <stdlib.h>
#include <util/delay.h>
#include <math.h> 

using namespace digitalcave;

extern Hsv hsv;
extern Matrix matrix;
extern Psx psx;

Mood::Mood() {
}

Mood::~Mood() {
}

void Mood::run() {
	uint8_t running = 1;
	uint16_t buttons;
	uint16_t changed;
	
	Hsv c = Hsv(hsv);
	// at values lower than this the color mixing doesn't work well
	if (c.getValue() < 0x0f) c.setValue(0x0f);

	uint8_t overflow = 128;
	
	const float k = 10.0;
	float v = 0.0;
	float xx;
	float yy;
	float cx;
	float cy;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	float time = 0;
	
	while (running > 0) {
		if (running == 1) {
			// solid color mood light
			matrix.rectangle(0,0,11,11,DRAW_FILLED);
			matrix.flush();

			c.addHue(1);
			matrix.setColor(Rgb(c));
		} else if (running > 1) {
			// plasma
			// http://www.bidouille.org/prog/plasma

			for (uint8_t x = 0; x < 12; x++) {
			 	xx = x/12.0-.5;
				for (uint8_t y = 0; y < 12; y++) {
					yy = y/12.0 - .5;
					v = sin((k*xx+time));
					v += sin((k*yy+time)/2.0);
					v += sin((k*xx+k*yy+time)/2.0);
					cx = (.5 * sin(time/5.0)) + xx;
					cy = (.5 * cos(time/3.0)) + yy;
			
					v += sin(sqrt(100.0*(cx*cx+cy*cy)+1.0)+time);
					v /= 2.0;
			
					if (running == 2) {
						r = 64.0*(.5+.5*sin(M_PI*v));
						g = 64.0*(.5+.5*cos(M_PI*v));
						b = 0;
					} else if (running == 3) {
						r = 64.0;
						g = 64.0*(.5+.5*cos(M_PI*v));
						b = 64.0*(.5+.5*sin(M_PI*v));
					} else if (running == 4) {
						r = 64.0*(.5+.5*sin(M_PI*v));
						g = 64.0*(.5+.5*sin(M_PI*v+2*M_PI/3));
						b = 64.0*(.5+.5*sin(M_PI*v+4*M_PI/3));
					} else if (running == 5) {
						float c = 64.0*(.5+.5*sin(M_PI*v*5.0));
						r = c;
						g = c;
						b = c;
					} else if (running == 6) {
						float c = 64.0*(.5+.5*v*0.8);
						r = 64.0*c;
						g = 64.0*c;
						b = 64.0*c;
					}
					matrix.setColor(r,g,b);
					matrix.setPixel(x,y);
				}
			}
			matrix.flush();
			time++;
		}
		
		psx.poll();
		buttons = psx.buttons();
		changed = psx.changed();
		if (buttons & PSB_PAD_DOWN && changed & PSB_PAD_DOWN) {
			overflow += 8;
		} else if (buttons & PSB_PAD_UP && changed & PSB_PAD_UP) {
			overflow -= 8;
		} else if (buttons & PSB_SELECT && changed & PSB_SELECT) {
			running++;
			running %= 6;
			if (running == 0) running = 1;
		}
		
		if (buttons & PSB_START && changed & PSB_START) {
			running = 0;
		}
		
		for (int i = 0; i < overflow; i++) {
			_delay_ms(1);
		}
	}
}