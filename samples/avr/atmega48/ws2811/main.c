#define WS2811_PORT PORTB

#include <avr/io.h>
#include <stdlib.h>
#include "lib/ws2811/ws2811_8.h"
#include <util/delay_basic.h>

int r(int max) {
	return rand() % max;
}
void a(int8_t p1, int8_t p2, int8_t *x) {
	if (p1 == p2) return;
	
	uint8_t p = p1 + p2;
	if (p2 < p1) p += 60;
	p /= 2;
	if (p == p1 || p == p2) return;
	p %= 60;
	
	uint8_t v = x[p1] + x[p2];
	if (x[p2] < x[p1]) v += 12;
	v /= 2;
	//v += r(3) - 1;
	if (v < 0) v = 11;
	if (v > 11) v = 0;
	
	x[p] = v;
	
	a(p1, p, x);
	a(p, p2, x);
}

int main (void) {

	TCCR0A = 0x00;
	TCCR0B = _BV(CS02) | _BV(CS00);

	ws2811_t red = {.red = 0xcc, .green = 0x00, .blue = 0x00 };			// 0
	ws2811_t green = {.red = 0x00, .green = 0xcc, .blue = 0x00 };		// 120
	ws2811_t blue = {.red = 0x00, .green = 0x00, .blue = 0xcc };		// 240

	ws2811_t yellow = {.red = 0xcc, .green = 0x66, .blue = 0x00 };		// 60
	ws2811_t cyan = {.red = 0x00, .green = 0xcc, .blue = 0x66 };		// 180
	ws2811_t magenta = {.red = 0xcc, .green = 0x00, .blue = 0x66 };		// 300

	ws2811_t orange = {.red = 0xcc, .green = 0x66, .blue = 0x00 };		// 30
	ws2811_t chartreuse = {.red = 0x66, .green = 0xcc, .blue = 0x00 };	// 90
	ws2811_t spring = {.red = 0x00, .green = 0xcc, .blue = 0x66 };		// 150
	ws2811_t azure = {.red = 0x00, .green = 0x66, .blue = 0xcc };		// 210
	ws2811_t violet = {.red = 0x66, .green = 0x00, .blue = 0xcc };		// 270
	ws2811_t rose = {.red = 0xcc, .green = 0x00, .blue = 0x66 };		// 330

	ws2811_t vermillion = {.red = 0xcc, .green = 0x33, .blue = 0x00 };	// 15
	ws2811_t amber = {.red = 0xcc, .green = 0x99, .blue = 0x00 };		// 45
	ws2811_t lime = {.red = 0x99, .green = 0xcc, .blue = 0x00 };		// 75
	ws2811_t harlequin = {.red = 0x33, .green = 0xcc, .blue = 0x00 };	// 105
	ws2811_t malachite = {.red = 0x00, .green = 0xcc, .blue = 0x33 };	// 135
	ws2811_t turquoise = {.red = 0x00, .green = 0xcc, .blue = 0x99 };	// 165
	ws2811_t cerulean = {.red = 0x00, .green = 0x99, .blue = 0xcc };	// 195
	ws2811_t sapphire = {.red = 0x00, .green = 0x33, .blue = 0xcc };	// 225
	ws2811_t indigo = {.red = 0x33, .green = 0x00, .blue = 0xcc };		// 255
	ws2811_t mulberry = {.red = 0x99, .green = 0x00, .blue = 0xcc };	// 285
	ws2811_t fuchsia = {.red = 0xcc, .green = 0x00, .blue = 0x99 };		// 315
	ws2811_t crimson = {.red = 0xcc, .green = 0x00, .blue = 0x33 };		// 345
	
	ws2811_t palette[24] = { red, vermillion, orange, amber, yellow, lime, chartreuse, harlequin, green, malachite, spring, turquoise, cyan, cerulean, azure, sapphire, blue, indigo, violet, mulberry, magenta, fuchsia, rose, crimson };

	struct ws2811_t colors[60];
	DDRB = _BV(1);
		
	for (uint8_t i = 0; i < 30; i++) {
		colors[(i*2)] = palette[i%24];
		colors[((i*2)+1)] = palette[i%24];
	}
	ws2811_set(colors, 60, 1);
	while (1) {
	}
}