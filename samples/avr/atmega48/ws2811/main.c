#include <avr/io.h>
#include "lib/ws2811/ws2811.h"
#include <util/delay.h>

int main (void) {

	//Do setup here
	struct ws2811_t colors[60];
	_delay_ms(500);
	
	//Main program loop
	while (1) {
		for (uint8_t i = 0; i < 60; i++) {
			for (uint8_t j = 0; j < 60; j++) {
				colors[j].red = 0x5;
				colors[j].green = 0x6;
				colors[j].blue = 0xb;
			}
			colors[i].red = 0x5c;
			colors[i].green = 0x61;
			colors[i].blue = 0xb4;

			ws2811_set(colors, 60);
			_delay_ms(255);	
		}
	}
}

