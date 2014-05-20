#include <avr/io.h>

#include "lib/remote/remote.h"
#include "lib/serial/serial.h"

int main (void){
	serial_init_b(9600);
	remote_init();
	
	uint8_t command;
	
	//Main program loop
	while (1){
		command = remote_get(); 
		if (command == REMOTE_MENU) UDR0 = 'M';
		else if (command == REMOTE_RIGHT) UDR0 = 'R';
		else if (command == REMOTE_LEFT) UDR0 = 'L';
		else if (command == REMOTE_UP) UDR0 = 'U';
		else if (command == REMOTE_DOWN) UDR0 = 'D';
		else if (command == REMOTE_CENTER) UDR0 = 'C';
		else if (command == REMOTE_A_CENTER) UDR0 = 'C';
		else if (command == REMOTE_A_PLAY) UDR0 = 'P';
	}
}

