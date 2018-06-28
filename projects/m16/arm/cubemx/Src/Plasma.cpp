#include "Plasma.h"
#include "Matrix.h"
#include <stdlib.h>
#include <util/delay.h>
#include <math.h>

using namespace digitalcave;

extern Matrix matrix;

Plasma::Plasma(uint8_t w, uint8_t h) : width(w), height(h) {
}

Plasma::~Plasma() {
}

void Plasma::paint() {
    if (frame > 0) {
        frame--;
    } else {
        frame = 5; // only re-paint every 5 frames
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

                if (color == 0) {
                    r = 64.0*(.5+.5*sin(M_PI*v));
                    g = 64.0*(.5+.5*cos(M_PI*v));
                    b = 0;
                } else if (color == 1) {
                    r = 64.0;
                    g = 64.0*(.5+.5*cos(M_PI*v));
                    b = 64.0*(.5+.5*sin(M_PI*v));
                } else if (color == 2) {
                    r = 64.0*(.5+.5*sin(M_PI*v));
                    g = 64.0*(.5+.5*sin(M_PI*v+2*M_PI/3));
                    b = 64.0*(.5+.5*sin(M_PI*v+4*M_PI/3));
                } else if (color == 3) {
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
}
