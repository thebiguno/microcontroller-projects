/*
 * Sample skeleton source file.
 */


#include <avr/io.h>

#include "lib/serialcpp/serial.h"

int main (void){
	Serial s(9600, 0, 3);
	
	//Main program loop
	for (;;){
	}
	
	return 0;
}
