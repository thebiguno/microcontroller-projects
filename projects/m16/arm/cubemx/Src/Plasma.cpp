#include "Plasma.h"
#include "Matrix.h"
#include <ButtonAVR.h>
#include <Rgb.h>
#include <stdlib.h>
#include <util/delay.h>
#include <math.h>

using namespace digitalcave;

extern uint32_t ms;
extern ButtonAVR b1;
extern ButtonAVR b2;

extern Matrix matrix;

Plasma::Plasma() {
}

Plasma::~Plasma() {
}

void Plasma::run() {
	uint8_t running = 1;

	uint8_t frame = 0;   // only paint 1 in every delay frames
	uint8_t delay = 5;   // the length of the delay

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
		_delay_ms(10);

		if (frame > 0) {
			frame--;
		} else {
			frame = delay;
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

					if (running == 1) {
						r = 64.0*(.5+.5*sin(M_PI*v));
						g = 64.0*(.5+.5*cos(M_PI*v));
						b = 0;
					} else if (running == 2) {
						r = 64.0;
						g = 64.0*(.5+.5*cos(M_PI*v));
						b = 64.0*(.5+.5*sin(M_PI*v));
					} else if (running == 3) {
						r = 64.0*(.5+.5*sin(M_PI*v));
						g = 64.0*(.5+.5*sin(M_PI*v+2*M_PI/3));
						b = 64.0*(.5+.5*sin(M_PI*v+4*M_PI/3));
					} else if (running == 4) {
						float c = 64.0*(.5+.5*sin(M_PI*v*5.0));
						r = c;
						g = c;
						b = c;
					} else {
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

		// handle buttons

		b1.sample(ms);
		b2.sample(ms);

		if (b1.longReleaseEvent()) {
			// exit
			running = 0;
		}	else if (b1.releaseEvent()) {
			// change plasma
			running++;
			running %= 6;
			if (running == 0) running = 1;
		} else if (b2.releaseEvent()) {
			// change speed
			delay += 5;
			if (delay > 20) {
				delay = 5;
			}
		}
	}
}
