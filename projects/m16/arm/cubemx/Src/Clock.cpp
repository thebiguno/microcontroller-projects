#include "Clock.h"
#include "ClockSet.h"
#include <ButtonAVR.h>
#include <Hsv.h>
#include <Rgb.h>
#include "Matrix.h"
#include <util/delay.h>
#include <stdlib.h>
#include "stm32f4xx_hal_rtc.h"

using namespace digitalcave;

extern uint32_t ms;
extern ButtonAVR b1;
extern ButtonAVR b2;

extern Hsv hsv;
extern Matrix matrix;

Clock::Clock() {
}

Clock::~Clock() {
}

void Clock::run() {
	mcp79410_time_t time;
	uint8_t running = 1;

	uint8_t field[4];
	char c[4];

	while (running) {
		_delay_ms(10);

		mcp79410_get(&time);

		hsv.setHue(time.second * 6);

		if (running == 1) {
			field[0] = time.hour / 10;
			field[1] = time.hour - (10 * field[0]);
			field[2] = time.minute / 10;
			field[3] = time.minute - (10 * field[2]);
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
			}
		}

		// draw
		matrix.setColor(0,0,0);
		matrix.rectangle(0,0,11,11,DRAW_FILLED);

		matrix.setColor(Rgb(hsv));
		matrix.character(0,0, c[0], 0);
		matrix.character(6,0, c[1], 0);
		matrix.setColor(Rgb(Hsv((hsv.getHue() + 180) % 360, hsv.getSaturation(), hsv.getValue())));
		matrix.character(0,6, c[2], 0);
		matrix.character(6,6, c[3], 0);

		matrix.flush();

		// handle buttons;

		b1.sample(ms);
		b2.sample(ms);

		if (b1.longReleaseEvent()) {
			// exit
			running = 0;
		} else if (b2.longReleaseEvent()) {
			// set date/time
			ClockSet cs = ClockSet();
			cs.run();
		}
	}
}
