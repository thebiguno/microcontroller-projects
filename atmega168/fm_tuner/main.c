#include <avr/io.h>
#include <util/delay.h>
#include "lib/fm/fm.h"

using namespace digitalcave;

int main (void){
	//Do setup here
	FM fm(&PORTD, PORTD2);

	fm.setVolume(2);
	fm.setChannel(977);
	
	//Main program loop
	while (1){
		_delay_ms(1000);
		fm.seek(1);
	}
}