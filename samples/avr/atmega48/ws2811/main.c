#define WS2811_PORT PORTB

#include <avr/io.h>
#include "lib/ws2811/ws2811_8.h"
#include <util/delay_basic.h>

int main (void) {

	//Do setup here
	struct ws2811_t colors[60];
	DDRB = _BV(1);
	
	//Main program loop
	while (1) {
		for (uint8_t i = 0; i < 60; i++) {
			for (uint8_t j = 0; j < 60; j++) {
				colors[j].red = 0x3;
				colors[j].green = 0x0;
				colors[j].blue = 0x2;
			}
			colors[i].red = 0x3c;
			colors[i].green = 0x06;
			colors[i].blue = 0x22;

			ws2811_set(colors, 60, 1);
		}
	}
}

