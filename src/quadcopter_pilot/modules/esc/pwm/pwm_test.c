#include <avr/io.h>

#include "../esc.h"

char temp[32];

int16_t x, last_x = 0;
int16_t y, last_y = 0;
int16_t z, last_z = 0;


int main (void){
	//Do setup here

	esc_init();

	//Main program loop
	while (1){

	}
}
