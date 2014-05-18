#include <avr/io.h>
#include "lib/ws2811/ws2811.h"

int main (void) {
	
	//Do setup here
	struct ws2811_t colors[60];
	for (uint8_t i = 0; i < 60; i++) {
		colors[i].red = 0xff;
		colors[i].green = 0x7f;
		colors[i].blue = 0x7f;
	}
	
	//Main program loop
	while (1) {
		ws2811_set(colors, 60);
	}
}

