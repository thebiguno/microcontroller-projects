#include "Plasma.h"
#include "Matrix.h"
#include <Buttons.h>
#include <Rgb.h>
#include <stdlib.h>
#include <util/delay.h>
#include <math.h> 

using namespace digitalcave;

extern uint8_t sample;
extern Matrix matrix;
extern Buttons buttons;

Plasma::Plasma() {
}

Plasma::~Plasma() {
}

void Plasma::run() {
	uint8_t running = 1;

	uint8_t overflow = 128;
	
	const float k = 10.0;
	float v = 0.0;
	float xx;
	float yy;
	float cx;
	float cy;
	uint8_t r;
	uint8_t g;
	float time = 0;
	
	while (running > 0) {
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
				} else if (running == 2) {
					r = 64.0;
					g = 64.0*(.5+.5*cos(M_PI*v));
				} else if (running == 3) {
					r = 64.0*(.5+.5*sin(M_PI*v));
					g = 64.0*(.5+.5*sin(M_PI*v+2*M_PI/3));
				} else if (running == 4) {
					float c = 64.0*(.5+.5*sin(M_PI*v*5.0));
					r = c;
					g = c;
				} else {
					float c = 64.0*(.5+.5*v*0.8);
					r = 64.0*c;
					g = 64.0*c;
				}
				matrix.setColor(r,g);
				matrix.setPixel(x,y);
			}
		}
		matrix.flush();
		time++;
	}
}