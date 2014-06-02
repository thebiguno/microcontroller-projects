#define WS2811_PORT PORTB

#include <avr/io.h>
#include "lib/ws2811/ws2811_8.h"
#include <util/delay_basic.h>

int main (void) {

	ws2811_t black = {.red = 0x00, .green = 0x00, .blue = 0x00 };
	ws2811_t red = {.red = 0xff, .green = 0x00, .blue = 0x00 };
	ws2811_t orange = {.red = 0xdd, .green = 0x33, .blue = 0x00 };
	ws2811_t yellow = {.red = 0xff, .green = 0xff, .blue = 0x00 };
	ws2811_t chartreuse = {.red = 0x33, .green = 0xdd, .blue = 0x00 };
	ws2811_t green = {.red = 0x00, .green = 0xff, .blue = 0x00 };
	ws2811_t spring = {.red = 0x00, .green = 0xdd, .blue = 0x33 };
	ws2811_t cyan = {.red = 0x00, .green = 0xff, .blue = 0xff };
	ws2811_t azure = {.red = 0x00, .green = 0x33, .blue = 0xdd };
	ws2811_t blue = {.red = 0x00, .green = 0x00, .blue = 0xff };
	ws2811_t violet = {.red = 0x33, .green = 0x00, .blue = 0xdd };
	ws2811_t magenta = {.red = 0xff, .green = 0x00, .blue = 0xff };
	ws2811_t rose = {.red = 0xdd, .green = 0x00, .blue = 0x33 };

	//Do setup here
	struct ws2811_t colors[60];
	DDRB = _BV(1);
	
	for (uint8_t i = 0; i < 60; i++) {
		if (i < 5) colors[i] = red;
		else if (i < 10) colors[i] = orange;
		else if (i < 15) colors[i] = yellow;
		else if (i < 20) colors[i] = chartreuse;
		else if (i < 25) colors[i] = green;
		else if (i < 30) colors[i] = spring;
		else if (i < 35) colors[i] = cyan;
		else if (i < 40) colors[i] = azure;
		else if (i < 45) colors[i] = blue;
		else if (i < 50) colors[i] = violet;
		else if (i < 55) colors[i] = magenta;
		else if (i < 60) colors[i] = rose;
		else colors[i] = black;
	}
	ws2811_set(colors, 60, 1);

	//Main program loop
	while (1) {
	}
}

