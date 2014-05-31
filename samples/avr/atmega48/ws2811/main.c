#define WS2811_PORT PORTB

#include <avr/io.h>
#include "lib/ws2811/ws2811_8.h"
#include <util/delay_basic.h>

int main (void) {

	ws2811_t black = {.red = 0x00, .green = 0x00, .blue = 0x00 };
	ws2811_t yellow = {.red = 0xcc, .green = 0xcc, .blue = 0x00 };
	ws2811_t orange = {.red = 0xcc, .green = 0x33, .blue = 0x00 };
	ws2811_t red = {.red = 0xff, .green = 0x00, .blue = 0x00 };
	ws2811_t magenta = {.red = 0x99, .green = 0x00, .blue = 0x99 };
	ws2811_t violet = {.red = 0x11, .green = 0x11, .blue = 0x66 };
	ws2811_t blue = {.red = 0x00, .green = 0x00, .blue = 0xff };
	ws2811_t cyan = {.red = 0x00, .green = 0x99, .blue = 0x99 };
	ws2811_t green = {.red = 0x00, .green = 0xff, .blue = 0x00 };
	ws2811_t base02 = {.red = 0x11, .green = 0x11, .blue = 0x11 };
	ws2811_t base2 = {.red = 0x99, .green = 0x99, .blue = 0x99 };

	//Do setup here
	struct ws2811_t colors[60];
	DDRB = _BV(1);
	
	for (uint8_t i = 0; i < 50; i++) {
		if (i < 5) colors[i] = yellow;
		else if (i < 10) colors[i] = orange;
		else if (i < 15) colors[i] = red;
		else if (i < 20) colors[i] = magenta;
		else if (i < 25) colors[i] = violet;
		else if (i < 30) colors[i] = blue;
		else if (i < 35) colors[i] = cyan;
		else if (i < 40) colors[i] = green;
		else if (i < 45) colors[i] = base02;
		else colors[i] = base2;
	}
	ws2811_set(colors, 60, 1);

	//Main program loop
	while (1) {
	}
}

