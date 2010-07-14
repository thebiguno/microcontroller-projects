/*
 * Sample skeleton source file.
 */

#include <avr/io.h>

#include "modules/control.h"
#include "modules/communication.h"

int main (void){
	//Init communication hardware
	init_communication();

	//Init control interface
	init_control();
	
	//Main program loop
	while (1){
		//Read controls
		

		//Send control data
	}
}

