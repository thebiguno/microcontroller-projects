#include <Animated.h>
#include <Matrix.h>
#include <lib/Psx/Psx.h>
#include <lib/Draw/Hsv.h>
#include <lib/Draw/Rgb.h>
#include <util/delay.h>

using namespace digitalcave;

	uint8_t sprites[96] PROGMEM = {
		// 8x8 space invader (orange)
		0x18, 0x3c, 0x7e, 0xdb, 0xff, 0x5a, 0x81, 0x42,
		0x18, 0x3c, 0x7e, 0xdb, 0xff, 0x24, 0x5a, 0xa5,

		// 8x8 space invader
		0x0, 0x3c, 0x7e, 0xdb, 0xdb, 0x7e, 0x24, 0xc3,
		0x3c, 0x7e, 0xdb, 0xdb, 0x7e, 0x24, 0x24, 0x24,

		// 8x8 space invader
		0x24, 0x24, 0x7e, 0xdb, 0xff, 0xff, 0xa5, 0x24,
		0x24, 0xa5, 0xff, 0xdb, 0xff, 0x7e, 0x24, 0x42,
		
		// 12x8 space invader (magenta)
		0x20, 0x41, 0x8, 0x3f, 0xe6, 0xf6, 0xff, 0xfb, 0xfd, 0xa0, 0x51, 0x98,
		0x20, 0x49, 0x9, 0xbf, 0xfe, 0xf7, 0xff, 0xf7, 0xfe, 0x20, 0x44, 0x2,

		// 12x8 space invader (green)
		0xf, 0x7, 0xfe, 0xff, 0xfe, 0x67, 0xff, 0xf1, 0x98, 0x36, 0xcc, 0x3,
		0xf, 0x7, 0xfe, 0xff, 0xfe, 0x67, 0xff, 0xf3, 0x9c, 0x66, 0x63, 0xc
	};
	
	
extern Matrix matrix;
extern Psx psx;
extern Hsv hsv;

Animated::Animated() {
}

Animated::~Animated() {
}

void Animated::run() {
	
	uint8_t running = 1;
	uint8_t state = 0;
	
	while (running > 0) {
		uint16_t buttons;
		uint16_t changed;
		
		matrix.setColor(0,0,0);
		matrix.rectangle(0,0,12,12,DRAW_FILLED);
		
		Hsv h = Hsv(hsv);
		if (running == 1) {
			h.setHue(30);
			matrix.setColor(Rgb(h));
			matrix.bitmap(2,2,8,8,DRAW_ORIENTATION_90,&sprites[(state & 0x04) ? 8 : 0]);
		} else if (running == 2) {
			h.setHue(0);
			matrix.setColor(Rgb(h));
			matrix.bitmap(2,2,8,8,DRAW_ORIENTATION_180,&sprites[(state & 0x04) ? 24 : 16]);
		} else if (running == 3) {
			h.setHue(240);
			matrix.setColor(Rgb(h));
			matrix.bitmap(2,2,8,8,DRAW_ORIENTATION_270,&sprites[(state & 0x04) ? 40 : 32]);
		} else if (running == 4) {
			h.setHue(300);
			matrix.setColor(Rgb(h));
			matrix.bitmap(0,2,12,8,DRAW_ORIENTATION_0,&sprites[(state & 0x04) ? 60 : 48]);
		} else if (running == 5) {
			h.setHue(120);
			matrix.setColor(Rgb(h));
			matrix.bitmap(0,2,12,8,DRAW_ORIENTATION_0,&sprites[(state & 0x04) ? 84 : 72]);
		}
		matrix.flush();
		
		psx.poll();
		buttons = psx.buttons();
		changed = psx.changed();
		
		if (buttons & PSB_PAD_UP && changed & PSB_PAD_UP ) {
			running++;
			if (running == 6) running = 1;
		} else if (buttons & PSB_PAD_DOWN && changed & PSB_PAD_DOWN) {
			running--;
			if (running == 0) running = 5;
		}
		
		if (buttons & PSB_TRIANGLE && changed & PSB_TRIANGLE) {
			running = 0;
		}
		
		_delay_ms(64);
		state++;
	}
}

