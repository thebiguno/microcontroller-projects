#include <avr/io.h>

#include "main.h"

int main (void){
	//Init communication hardware
	communication_init();

	//Init control interface
	control_init();
	
	//Main program loop
	while (1){
		//Read controls
		

		//Send control data
	}
}

