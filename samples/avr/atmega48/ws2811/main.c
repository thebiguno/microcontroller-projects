#define WS2811_PORT PORTB

#include <avr/io.h>
#include <stdlib.h>
#include "lib/ws2811/ws2811_8.h"
#include <util/delay_basic.h>

uint8_t r(uint8_t max) {
	return rand() / (RAND_MAX / max + 1);
}
void a(uint8_t p1, uint8_t p2, uint8_t *x) {
	if (p1 == p2) return;
	
	uint8_t p = p1 + p2;
	if (p2 < p1) p += 60;
	p /= 2;
	if (p == p1 || p == p2) return;
	p %= 60;
	
	uint8_t v = x[p1] + x[p2];
	if (x[p2] < x[p1]) v += 12;
	v /= 2;
	//v = 1;
	//v += (r(2) - 1);
	//v %= 60;
	x[p] = v;
	
	a(p1, p, x);
	a(p, p2, x);
}

int main (void) {

	TCCR0A = 0x00;
	TCCR0B = _BV(CS02) | _BV(CS00);

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

	ws2811_t palette[12] = { red, orange, yellow, chartreuse, green, spring, cyan, azure, blue, violet, magenta, rose };
	uint8_t x[60];
	
	// start with random colors at random locations
	uint8_t p1 = 2;
	uint8_t p2 = 22;
	uint8_t p3 = 42;
	x[p1] = 0;
	x[p2] = 4;
	x[p3] = 8;
	
	struct ws2811_t colors[60];
	DDRB = _BV(1);
	
	ws2811_set(colors, 60, 1);
	
	while (1) {
		a(p1, p2, x);
		a(p2, p3, x);
		a(p3, p1, x);

		for (uint8_t i = 0; i < 60; i++) {
			colors[i] = palette[x[i]];
		}
		ws2811_set(colors, 60, 1);
		
		p1 += 1;//(((int8_t) r(2)) - 1);
		p2 += 1;//(((int8_t) r(2)) - 1);
		//p3 += 1;//(((int8_t) r(2)) - 1);
		if (p1 < 0) p1 = 59;
		if (p2 < 0) p2 = 59;
		if (p3 < 0) p3 = 59;
		if (p1 > 59) p1 = 0;
		if (p2 > 59) p2 = 0;
		if (p3 > 59) p3 = 0;
		
		while (TCNT0 < 0xff) {
			;
		}
		TCNT0 = 0;
	}
}