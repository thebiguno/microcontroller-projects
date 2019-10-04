#include <avr/io.h>
#include <util/delay.h>
#include <Serial/SerialUSB.h>
#include "SI4703/SI4703.h"

using namespace digitalcave;

SerialUSB serial;

int main (void){
	//Do setup here

	_delay_ms(2000);
	serial.write("Starting\n\r");
	Si4703_Breakout fm(&PORTD, PORTD2);

	char temp[10];
	fm.setVolume(2);
	fm.setChannel(977);

	//Main program loop
	while (1){
		_delay_ms(5000);
//		fm.setChannel(977);
		uint16_t chan = fm.seekUp();
		serial.write(itoa(chan, temp, 10));
		serial.write("\r\n");
	}
}
