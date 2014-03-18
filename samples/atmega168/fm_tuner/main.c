#include <avr/io.h>
#include <util/delay.h>
#include "lib/si4703/si4703.h"
#include "lib/serial/serial.h"

using namespace digitalcave;

int main (void){
	//Do setup here
	serial_init_b(9600);
	Si4703_Breakout fm(&PORTD, PORTD2);

	char temp[10];
	fm.setVolume(2);
	fm.setChannel(977);
	
	//Main program loop
	while (1){
		_delay_ms(5000);
//		fm.setChannel(977);
		uint16_t chan = fm.seekUp();
		serial_write_s(itoa(chan, temp, 10));
		serial_write_s("\r\n");
	}
}